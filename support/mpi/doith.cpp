
//  This code is based on the example at:
//  http://www.lam-mpi.org/tutorials/one-step/ezstart.php

#include <fstream>
#include <iostream>
#include <iterator>    // for std::istream_iterator
#include <libgen.h>
#include <mpi.h>
#include <sstream>     // for std::istringstream
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <sys/wait.h>  // for wait4pid
#include <time.h>
#include <vector>      // for std::vector
#include <unistd.h>


#define WORKTAG     1
#define DIETAG      2
#define TASK_LENGTH 2047


using namespace std;

extern char   **environ;


string          g_EMPTY("");
int             g_rflag        = 0;
char           *g_rank_string = NULL;
char           *g_task_list   = NULL;
bool            g_verbose      = false;
ifstream        g_tasks_file;
int             g_rank;
int             g_num_procs;
int             g_line_number;



static bool     can_exec(string const & p_command);
static void     do_work(void);
static string   get_next_work_item(void);
static void     process_args(int argc, char **argv);
static void     timestamp(string const & p_message);
static void     usage(string const & p_message);


static void
timestamp(string const & p_message)
{
  if (g_verbose)
  {
    time_t ltime = time(NULL);
    cerr << p_message << " " << asctime(localtime(&ltime));
  }
}


static void
usage(string const & p_message)
{
  string  myname;
  char    buff[1024];
  ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1);
  int     status = EXIT_SUCCESS;

  if (len != -1)
  {
    buff[len] = '\0';
    myname = basename(buff);
  }
  else
  {
    //  Handle error condition
    myname = "Could not get this executable name!";
  }

  if (0 < p_message.size())
  {
    cerr << endl << p_message << endl;
    status = EXIT_FAILURE;
  }
  cerr << "\n";
  cerr << "  Usage: " << myname << " -t task_list [-r string] [-v]\n";
  cerr << "\n";
  cerr << "         where task_list is a file that contains a list of tasks.\n";
  cerr << "         Each line in task_list must be a standalone command that\n";
  cerr << "         could be run on its own from the command line.\n";
  cerr << "\n";
  cerr << "         If you specify the -r option, each task in the task_list will\n";
  cerr << "         will be appended with -string and the rank of the worker.\n";
  cerr << "\n";
  cerr << "         For example, lets say we have a task_list named run_search.txt\n";
  cerr << "         with lines that look:\n";
  cerr << "            /path_to_exe/search.pl -i 101\n";
  cerr << "            /path_to_exe/search.pl -i 102\n";
  cerr << "\n";
  cerr << "         If you specified the command line to look like:\n";
  cerr << "            " << myname << " -t run_search.txt -r RANK\n";
  cerr << "         Then " << myname << " would read line one of run_search.txt\n";
  cerr << "            and run the following command on a compute node 1:\n";
  cerr << "              /bin/sh -c /path_to_exe/search.pl -i 101 -RANK 1\n";
  cerr << "            and run the following command on a compute node 2:\n";
  cerr << "              /bin/sh -c /path_to_exe/search.pl -i 101 -RANK 2\n";
  cerr << endl;

  exit(status);
}


static bool
can_exec(string const & p_command)
{
  struct  stat st;

  //  Find the first string (file) and see if it is an executable
  //
  istringstream             iss(p_command);
  istream_iterator<string>  begin(iss);
  istream_iterator<string>  end;
  vector<string>            tokens(begin, end);
  string                    file = (*tokens.begin());

  //  Is file hardcoded to an executable?
  //
  if (stat(file.c_str(), &st) >= 0)
  {
    if ((st.st_mode & S_IEXEC) != 0)
    {
      return(true);
    }
  }

  char   *search_path = getenv("PATH");
  char   *dir;
  string  executable;

  //  Search for "file" in the user's path
  //
  for (dir = strtok(search_path, ":"); dir; dir = strtok(NULL, ":"))
  {
    string         check(dir);
    executable   = check + "/" + file;

//cerr << "CHECKING " << executable << endl;
    if (stat(executable.c_str(), &st) >= 0)
    {
//cerr << "FOUND    " << executable << endl;
      if ((st.st_mode & S_IEXEC) != 0)
      {
//cerr << "YEEHAW   " << executable << endl;
        return(true);
      }
    }
  }

  return(false);
}


static void
process_args(int argc, char **argv)
{
  int     index;
  int     opt;
  ssize_t len;
  char    option[1024];
  string  error_message("");

  opterr = 0;

  //  http://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html
  //
  while ((opt = getopt(argc, argv, "hr:t:v")) != -1)
  {
    len = sprintf(option, "%c", optopt);
    if (len != -1) { option[len] = '\0'; }
    
    switch (opt)
    {
      case 'h':
        usage("");
        break;
      case 'r':
        g_rflag       = 1;
        g_rank_string = optarg;
        break;
      case 't':
        g_task_list   = optarg;
        if (access(g_task_list, R_OK) == -1)
        {
          error_message += "Can't read task_list, " + string(g_task_list) + "\n";
        }
        break;
      case 'v':
        g_verbose     = true;
        break;
      case '?':
        if (optopt == 'r')
        {
          error_message += "Option -r requires an argument.\n";
        }
        else if (optopt == 't')
        {
          error_message += "Option -t requires an argument.\n";
        }
        else if (isprint(optopt))
        {
          error_message += "Unknown option `-" + string(option) + "'.\n";
        }
        else
        {
          error_message += "Unknown option character `\\x" + string(option) + "'.\n";
        }
        break;
    }
  }  

  if (
      (NULL == g_task_list)                   ||
      (NULL == g_rank_string && 1 == g_rflag) || 
      (0    <  error_message.size())
     )
  {
    usage("FATAL ERROR: " + error_message);
  }

  for (index = optind; index < argc; index++)
  {
    cerr << "WARNING: Non-option argument " << argv[index] << endl;
  }

  return;
}


int
main(int argc, char **argv)
{
  timestamp("DOITH START:");
  int check = MPI_Init(&argc, &argv);
  g_line_number = 0;

  process_args(argc, argv);

  if (MPI_SUCCESS != check)
  {
    cerr << "FATAL ERROR: MPI_Init failed and returned: " << check << endl;
    return(EXIT_FAILURE);
  }

  //  Find out my identity in the default communicator
  MPI_Comm_rank(MPI_COMM_WORLD, &g_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &g_num_procs);

  do_work();

  MPI_Finalize();
  timestamp("DOITH STOP :");

  exit(EXIT_SUCCESS);
  return(EXIT_SUCCESS);
}


static void
do_work(void)
{
  int    status     = -1;
  string command;
  bool   executable = false;

  g_tasks_file.open(g_task_list);

  while (1)
  {
    command = get_next_work_item();
    if (command.empty())
    {
//cerr << "WTF" << endl;
      return;
    }
  
    if (g_rflag)
    {
      ostringstream oss("");
      oss << g_rank;
      command += string(" -") + string(g_rank_string) + " " + oss.str();
    }
  
    if (can_exec(command))
    {
      executable = true;
//cerr << "GOTO RUN ON:" << g_rank << ":" << command << ":" << endl;
      status     = system(command.c_str());
    }

    ostringstream oss("");
    oss << status ;
    if (g_verbose)
    {
      if (status >= 0)
      {
        cerr << "RAN COMMAND:";
      }
      else
      {
        if (executable)
        {
          cerr << "FAILED COMMAND:";
        }
        else
        {
          cerr << "CANNOT EXECUTE COMMAND:";
        }
      }
      cerr << command << ": EXIT STATUS:" << status << ":" << endl;
    }
  }
}


static string 
get_next_work_item(void)
{
  string task;
  while (g_tasks_file.good())
  {
    getline(g_tasks_file, task);
//cerr << "RANK:" << g_rank << ": PROCS:" << g_num_procs << ": TASK:" << task << ":" << endl;
    if (g_tasks_file.fail())
    {
//      cerr << "error from getline in get_next_work_item" << endl;
      break;
    }
    if (g_line_number % g_num_procs == g_rank)
    {
//cerr << "LINE:" << g_line_number << ": RANK:" << g_rank << ": PROCS:" << g_num_procs << ":" << endl;
      ++g_line_number;
      return(task);
    }
//cerr << "LINE:" << g_line_number << ": RANK:" << g_rank << ": PROCS:" << g_num_procs << ":" << endl;
    ++g_line_number;
  }
  g_tasks_file.close();
//cerr << "NO WORK LEFT" << endl;
  return(g_EMPTY);
}
