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


//  DEVELOPER NOTE:  For Performance to turn off all BOOST assertions
#define BOOST_DISABLE_ASSERTS

// System Includes
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
// External Includes
#include <boost/algorithm/string/regex.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
// Internal Includes
// Application Includes
#include "AlignmentsCollection.hpp"
#include "ReportRipper.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "AlignmentsCollection"


// Namespaces used
using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace boost::filesystem;
using boost::lexical_cast;


// Constructors


AlignmentsCollection::AlignmentsCollection(sharedLookup const & p_lookup) throw() :
lookup(p_lookup),
collection(collection_type()) 
{
  return;
}


// Operators


// Public Methods


// equalize all hit scores:
//  example: assume A_7 vs B_5 hit_score = 12, and
//                  B_5 vs A_7 hit_score = 14
//           set A_7 vs B_5 and B_5 vs A_7 hit_score to (12 + 14) / 2 = 13
void AlignmentsCollection::equalize(void) throw()
{
  map<id_type, map<id_type, float> >::iterator itor = num_to_score.begin();
  map<id_type, map<id_type, float> >::iterator etor = num_to_score.end();

  for (itor = itor; itor != etor; ++itor)
  {
    map<id_type, float>::iterator iitor = (*itor).second.begin();
    map<id_type, float>::iterator ietor = (*itor).second.end();
    for (iitor = iitor; iitor != ietor; ++iitor)
    {   
      float scoreAB = num_to_score[(*itor).first][(*iitor).first];
      float scoreBA = num_to_score[(*iitor).first][(*itor).first];
      int truncated = (int)(((scoreAB + scoreBA) / 2.0) + 0.5); //equalize, rounding up

      addScore((*itor).first, (*iitor).first, (float)truncated);
      addScore((*iitor).first, (*itor).first, num_to_score[(*itor).first][(*iitor).first]);
    }
  }

  return;
}


// read or generate *.hits files
void AlignmentsCollection::getAlignments(vector< tuple<string, species_type, species_type> > const & p_out_files) throw()
{
  vector< tuple<string, species_type, species_type> >::const_iterator itor = p_out_files.begin();
  vector< tuple<string, species_type, species_type> >::const_iterator etor = p_out_files.end();
  string blast_ext(".blastout");

  for (itor = itor; itor != etor; ++itor)
  {
    string hit_file       = (*itor).get<0>();
    species_type spec_1   = (*itor).get<1>();
    species_type spec_2   = (*itor).get<2>();
    string blast_out_file = hit_file + blast_ext;
    hit_file             += ".hits";

    if ((! exists(hit_file))              ||
        (  file_size(hit_file) == 0)      ||
        (last_write_time(hit_file) <
         last_write_time(blast_out_file))
       )
    {
      //  Generate the "Hit" file
      ::g_output_helper->write(e_journal, "Parsing BLAST output file:" + blast_out_file + "\n");
      ReportRipper ripper(blast_out_file, lookup);
      ofstream hit_file_stream;
      hit_file_stream.open(hit_file.c_str());
      hit_file_stream << ripper;
      hit_file_stream.close();
    }
    else
    {
      ::g_output_helper->write(e_journal, "Hit   output file already exists: " + hit_file + "\n");
    }

    //  Read in the "Hit" file
    if (! exists(hit_file)) 
    {
      FATAL("Find Hit file", "Read in Hit file", "Could not find hitfile: " + hit_file + "\n");
    }
    if (file_size(hit_file) == 0)
    {
      WARN("Find Hit file", "Read in Hit file", "Empty hitfile: " + hit_file + "\n");
    }

    addHitFile(spec_1, spec_2, hit_file);

    if ((spec_1 == e_species_B) && (spec_2 == e_species_A))
    {
      //lookup->equalize();
      equalize();
    }
  }

  return;
}


//  Read in lines from the "hits" file generated from parsing the blast output
void AlignmentsCollection::addHitFile(species_type const p_species_1, species_type const p_species_2, string const & p_filename) throw()
{
  ifstream myfile(p_filename.c_str());
  string line;

  if (myfile.is_open())
  {
    while (myfile.good() && (! myfile.eof()) && myfile.is_open())
    {
      getline (myfile,line);
 
      vector<string> sv;
      split_regex(sv, line, regex("[[:space:]]+"));

      if (sv.size() < 2) continue; // if the line is empty, sv will have 1 item in it

      id_type qid    = lookup->getIdByName(sv[0]);
      id_type sid    = lookup->getIdByName(sv[1]);


      istringstream os(sv[2]);
      float   score;
      os >> score;
      //  NOTE: We need integer/float scores from the blast tabular report
      //        to be rounded up to match what the inparanoid script does
      //        when reading float scores from the blast XML file. If the
      //        tabular report always printed out floating point scores then
      //        the rounding could be removed.
      int tmp = (int)(score + 0.5);
      score = (float)tmp;

      //  qlen and slen are the query and subject lengths in the FASTA file
      //  qam  and sam  are the query and subject aggregate match lengths
      //  qlm  and slm  are the query and subject local match lengths
      u_int32_t qlen = lexical_cast<u_int32_t>(sv[3]);
      u_int32_t slen = lexical_cast<u_int32_t>(sv[4]);
      u_int32_t qam  = lexical_cast<u_int32_t>(sv[5]);
      u_int32_t sam  = lexical_cast<u_int32_t>(sv[6]);
      u_int32_t qlm  = lexical_cast<u_int32_t>(sv[7]);
      u_int32_t slm  = lexical_cast<u_int32_t>(sv[8]);


      //  IMPORTANT: The segment  length must be > getSegmentCoverageCutoff AND
      //             The sequence length must be > getSeqOverlapCutoff
      //             This IS NOT the way InParanoid works

      // Is this query and subject sequence big enough to keep?
      if ((qam < (lookup->getSeqOverlapCutoff() * (float)qlen)) ||
          (sam < (lookup->getSeqOverlapCutoff() * (float)slen)))
      {
        continue;
      }

      // Is this query and subject segments big enough to keep?
      if ((qlm < (lookup->getSegmentCoverageCutoff() * (float)qlen)) ||
          (slm < (lookup->getSegmentCoverageCutoff() * (float)slen)))
      {
        continue;
      }

      // Is the score of this alignment good enough to keep?
      if (score < lookup->getScoreCutoff()) 
      {
        continue;
      }

      myKey key(p_species_1, p_species_2);

      collection_type::const_iterator ftor = collection.find(key);
      collection_type::const_iterator etor = collection.end();

      if (ftor != etor) // species1-species2 key already exists
      {
        qid_map::iterator qitor = collection[key].find(qid);
        qid_map::iterator qetor = collection[key].end();

        if (qitor != qetor) // query already exists
        {
          (*qitor).second->add(sid, score);
        }
        else //need to add the query and create a new Alignments object
        {
          sharedAlignments alignment(new Alignments(p_species_1, p_species_2, qid, sid, score));
          collection[key].insert(pair<id_type, sharedAlignments>(qid, alignment));
        }
      }
      else // add new key
      {
        qid_map tmp;
        sharedAlignments alignment(new Alignments(p_species_1, p_species_2, qid, sid, score));
        tmp.insert(pair<id_type, sharedAlignments>(qid, alignment));
        collection.insert(pair< myKey, qid_map >(key, tmp));
      }

      addScore(qid, sid, score);
      // if (cross hit)
      if (((p_species_1 == e_species_A) && (p_species_2 == e_species_B)) ||
          ((p_species_1 == e_species_B) && (p_species_2 == e_species_A)))
      {
        if (getScore(qid, sid) < lookup->getScoreCutoff())
        {
          addScore(sid, qid, score);
        }
      }
    }
    myfile.close();
  }

  return;
}


// returns all the hits against p_id with p_species better than p_score
vector<id_type> AlignmentsCollection::getHitsBetterThan(species_type const p_species, id_type const p_id, float const p_score) throw()
{
  myKey key(p_species, p_species);
  collection_type::const_iterator ftor  = collection.find(key);
  collection_type::const_iterator etor  = collection.end();

  if (ftor != etor)
  {
    qid_map::const_iterator qftor = (*ftor).second.find(p_id);
    qid_map::const_iterator qetor = (*ftor).second.end();

    if (qftor != qetor)
    {
      return((*qftor).second->getHitsBetterThan(p_score));
    }
  }

  vector<id_type> empty;
  return(empty);
}


// find orthologs (reciprocal best hits)
void AlignmentsCollection::findOrthologs(void) throw()
{
  //look for A:B
  myKey ABkey(e_species_A, e_species_B);
  collection_type::const_iterator ftor  = collection.find(ABkey);
  //collection_type::const_iterator etor  = collection.end();

  //look for B:A
  myKey BAkey(e_species_B, e_species_A);
  collection_type::const_iterator sftor = collection.find(BAkey);
  //collection_type::const_iterator setor = collection.end();

  qid_map::const_iterator qitor = (*ftor).second.begin();
  qid_map::const_iterator qetor = (*ftor).second.end();
  
  for (qitor = qitor; qitor != qetor; ++qitor)
  {
    id_type query = (*qitor).first;

    //get the best hits on this query
    vector< pair<id_type, float> > best_hits = (*qitor).second->getBestHits();
    vector< pair<id_type, float> >::const_iterator spitor = best_hits.begin();
    vector< pair<id_type, float> >::const_iterator spetor = best_hits.end();

    for (spitor = spitor; spitor != spetor; ++spitor)
    {
      id_type subject = (*spitor).first;
      float   score   = (*spitor).second;

      qid_map::const_iterator hitor = (*sftor).second.find((*spitor).first);
      // make sure there is a reciprocal hit
      if (hitor == (*sftor).second.end())       continue;
      // make sure there is a reciprocal best hit
      if (! (*hitor).second->isABestHit(query)) continue;

      // filter by outgroup
      myKey ACkey(e_species_A, e_outgroup);
      collection_type::const_iterator oftor  = collection.find(ACkey);
      collection_type::const_iterator oetor  = collection.end();
      bool bad = false;
      stringstream rejection_str("");
      rejection_str << "Ortholog pair (" << lookup->getNameById(query) << "-" << lookup->getNameById(subject) << ").\n";
 
      if (oftor != oetor)
      {
        qid_map::const_iterator itor1 = (*oftor).second.find(query);
        qid_map::const_iterator etor1 = (*oftor).second.end();

        if (itor1 != etor1)
        {
          float  best      = (*itor1).second->getBestScore();
          string best_name = lookup->getNameById((*itor1).second->getBestHit());

          if (best - score > lookup->getOutgroupCutoff())
          {
            bad = true;
            rejection_str << "   " << lookup->getNameById(query) << " is closer to " << best_name << " than to " << lookup->getNameById(subject) << "\n";
            rejection_str << "   " << score << " < " << best << " by " << (best - score) << "\n";
          }
        }
      }

      myKey BCkey(e_species_B, e_outgroup);
      oftor  = collection.find(BCkey);
      oetor  = collection.end();

      if (oftor != oetor)
      {
        qid_map::const_iterator itor1 = (*oftor).second.find(subject);
        qid_map::const_iterator etor1 = (*oftor).second.end();

        if (itor1 != etor1)
        {
          float  best      = (*itor1).second->getBestScore();
          string best_name = lookup->getNameById((*itor1).second->getBestHit());

          if (best - score > lookup->getOutgroupCutoff())
          {
            bad = true;
            rejection_str << "   " << lookup->getNameById(subject) << " is closer to " << best_name << " than to " << lookup->getNameById(query) << "\n";
            rejection_str << "   " << score << " < " << best << " by " << (best - score) << "\n";
          }
        }
      }

      if (bad) 
      {
        ::g_output_helper->write(e_rejects, rejection_str.str());
      }
      else
      { 
        addOrtho(query, (*hitor).first); 
      }
    }
  }

  return;
}


// for debugging
string AlignmentsCollection::printOrthologs(void) throw()
{
  stringstream output;

  list<Ortholog> temp = getOrthologs();
  list<Ortholog>::const_iterator itor = temp.begin();
  list<Ortholog>::const_iterator etor = temp.end();

  for (itor = itor; itor != etor; ++itor)
  {
    output << (*itor).getIdA() + 1;
    output << "\t";
    output << (*itor).getIdB() - 4624;
    output << "\t";
    output << (*itor).getScore();
    output << "\t";
    output << lookup->getNameById((*itor).getIdA());
    output << "\t";
    output << lookup->getNameById((*itor).getIdB());
    output << "\n";
  }

  return(output.str());
}



string AlignmentsCollection::print(void) const throw()
{
  return("AlignmentsCollection::print - NOT Implemented Yet\n");
}


ostream & operator<<(ostream & p_os, AlignmentsCollection const * p_collection) throw()
{
  return(p_os << p_collection->print());
}


ostream & operator<<(ostream & p_os, AlignmentsCollection const & p_collection) throw()
{
  return(p_os << p_collection.print());
}
