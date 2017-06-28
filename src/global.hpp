///////////////////////////////////////////////////////////////////////////////
// Copyright © 2013, Battelle Memorial Institute
//
// THIS FILE INITIALLY CREATED WITH:
//     TEMPLATE NAME:  lang_cpp_hdr.template
//     COMMAND NAME:   gensrc
//
// CODING CONVENTIONS:
//    * Class names are CamelCase with first word upper case
//    * Functions are camelCase with first word lower case
//    * Function parameters are lower case with _ and have p_ prefix
//    * Member variables always use 'this' pointer
///////////////////////////////////////////////////////////////////////////////


#ifndef GLOBAL_HPP
#define GLOBAL_HPP


// System Includes
#include <map>
#include <string>
#include <vector>
// External Includes
#include "boost/shared_ptr.hpp"
#include "boost/tuple/tuple_io.hpp"
// Internal Includes
// Application Includes
#include "osUtil.hpp"
#include "OutputHelper.hpp"


// DEFINITIONS

#define APP_NAME_LOWER "spocs"
#define APP_NAME_UPPER "SPOCS"
#define APP_DESC "Species Paralogy and Orthology Clique Solver (" APP_NAME_LOWER ")"

#define FATAL(p_ecode, p_item, p_context) message(p_ecode, p_item, p_context, __CLASS__, __func__, __FILE__, __LINE__, EXIT_FAILURE)
#define WARN(p_ecode,  p_item, p_context) message(p_ecode, p_item, p_context, __CLASS__, __func__, __FILE__, __LINE__, EXIT_SUCCESS)


// Namespaces
using namespace std;
using namespace boost;


extern double              const g_nat_log_2;
extern map<char, int>            g_clique_types;
extern sharedOutputHelper        g_output_helper;


typedef u_int32_t id_type;


typedef enum
{
  e_species_A,
  e_species_B,
  e_outgroup
} species_type;


typedef enum
{
    Score,
    Vertex1,
    Vertex2
} edges_t;


typedef enum
{
  e_in,           //  The output file for the HTML version of the report
  e_out,          //  The output file for the HTML version of the report
  e_err,          //  The output file for the HTML version of the report
  e_html,         //  The output file for the HTML version of the report
  e_log,          //  The output of spocs
  e_journal,      //  The output file for human readable steps taken during processing
  e_ortholog,     //  The output file for Orthologs - mostly for debugging
  e_paralog,      //  The output file for paralog clusters
  e_rejects,      //  The output file for rejected orthologs that were closer to outgroup rather than another species
  e_report,       //  The output file for paralog clusters
  e_badparalogs,  //  The output file for rejected paralog clusters
  e_script,       //  The output file for script to run blastall
} output_file_t;


typedef tuple<int, int, int>   Edge_t;
typedef vector< Edge_t >       Edges_t;
typedef boost::shared_ptr< Edges_t >  sharedEdges;


#undef  ENUM_OR_STRING
#define GRAPH_TYPES                       \
    ENUM_OR_STRING( Complete ),           \
    ENUM_OR_STRING( Semicomplete ),       \
    ENUM_OR_STRING( Incomplete ),         \
    ENUM_OR_STRING( Degenerate ),         \
    ENUM_OR_STRING( Unknown ),            \
    ENUM_OR_STRING( IncompleteSolved ),   \
    ENUM_OR_STRING( DegenerateSolved ),   \
    ENUM_OR_STRING( Last )

#define RAINBOW                           \
    ENUM_OR_STRING( red              ),   \
    ENUM_OR_STRING( orange           ),   \
    ENUM_OR_STRING( yellow           ),   \
    ENUM_OR_STRING( green            ),   \
    ENUM_OR_STRING( blue             ),   \
    ENUM_OR_STRING( indigo           ),   \
    ENUM_OR_STRING( violet           ),   \
    ENUM_OR_STRING( MAX_COLOR        )

#undef  ENUM_OR_STRING
#define ENUM_OR_STRING( name ) name


typedef enum graph_type
{
    GRAPH_TYPES
} graph_type_t;


typedef enum color_type
{
    RAINBOW
} color_type_t;


#undef  ENUM_OR_STRING


// Getters/Setters

const char* getGraphTypeStr(graph_type_t p_type) throw();
const char* getColorTypeStr(color_type_t p_type) throw();
void        setType(char type) throw();
bool        isTypeSet(graph_type_t type) throw();


// Functions

void   message(string const & p_ecode,
               string const & p_item,
               string const & p_context,
               string const & p_class,
               string const & p_func,
               string const & p_file,
               int    const   p_line,
               int    const   p_status) throw();

void   global_init(void) throw();
string getParalogFileExt(void) throw();
string getSpeciesFromPath(string const & p_path) throw();
string getTimestamp(void) throw();
void   splitTitle(string const title, string & name, string & comment) throw();
bool getWriteBLASTScriptsOnly();
void setWriteBLASTScriptsOnly(bool);
double string_to_double( const std::string& s );


#endif // GLOBAL_HPP
