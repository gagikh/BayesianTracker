/*
--------------------------------------------------------------------------------
 Name:     BayesianTracker
 Purpose:  A multi object tracking library, specifically used to reconstruct
           tracks in crowded fields. Here we use a probabilistic network of
           information to perform the trajectory linking. This method uses
           positional and visual information for track linking.

 Authors:  Alan R. Lowe (arl) a.lowe@ucl.ac.uk

 License:  See LICENSE.md

 Created:  14/08/2014
--------------------------------------------------------------------------------
*/

// TODO(arl): clean up the stdout

#include "manager.h"

bool compare_hypothesis_time(const Hypothesis &h_one, const Hypothesis &h_two)
{
  return h_one.trk_ID->track[0]->t < h_two.trk_ID->track[0]->t;
}



// take two tracks and merge the contents, rename the merged track and mark for
// removal at the end
void join_tracks(const TrackletPtr &parent_trk, const TrackletPtr &join_trk)
{
  if (DEBUG) std::cout << join_trk->ID << ",";

  // append the pointers to the objects to the parent track object
  for (size_t i=0; i<join_trk->length(); i++) {
    parent_trk->append(join_trk->track[i], true);
  }

  // set the renamed ID and a flag to remove
  join_trk->renamed_ID = parent_trk->ID;
  join_trk->to_remove(true);

  // set the fate of the parent track to that of the joined track
  parent_trk->fate = join_trk->fate;
}



// branches
void branch_tracks(const BranchHypothesis &branch)
{
  // makes some local pointers to the tracklets
  TrackletPtr parent_trk = std::get<0>(branch);
  TrackletPtr child_one_trk = std::get<1>(branch);
  TrackletPtr child_two_trk = std::get<2>(branch);

  // local ID for parent track (could be renamed, so need to check)
  unsigned int ID;

  // first check to see whether the parent has been renamed
  if (parent_trk->to_remove()) {
    ID = parent_trk->renamed_ID;
  } else {
    ID = parent_trk->ID;
  }

  // output some details?
  if (DEBUG) {
    std::cout << parent_trk->ID << " (renamed: " << ID << ") {";
    std::cout << child_one_trk->ID << ", ";
    std::cout << child_two_trk->ID << "}";
  }

  // set the parent ID for these children
  child_one_trk->parent = ID;
  child_two_trk->parent = ID;

  // TODO(arl): we can also set children here, this makes tree generation easier

  // set the fate of the parent as 'divided'
  parent_trk->fate = TYPE_Pdivn;
}



// take a list of hypotheses and re-organise the tracks following optimisation
void TrackManager::merge(const std::vector<Hypothesis> &a_hypotheses)
{

  if (a_hypotheses.empty()) {
    if (DEBUG) std::cout << "Hypothesis list is empty!" << std::endl;
    return;
  }

  if (empty()) {
    if (DEBUG) std::cout << "Track manager is empty!" << std::endl;
    return;
  }

  // get the number of hypotheses
  size_t n_hypotheses = a_hypotheses.size();

  // make some space for the different hypotheses
  m_links = HypothesisMap<JoinHypothesis>( size() );
  m_branches = HypothesisMap<BranchHypothesis>( size() );

  // loop through the hypotheses, split into link and branch types
  for (size_t i=0; i<n_hypotheses; i++) {

    Hypothesis h = a_hypotheses[i];

    // set the fate of each track as the 'accepted' hypothesis. these will be
    // overwritten in the link and division events
    h.trk_ID->fate = h.hypothesis;

    switch (h.hypothesis) {

      // linkage
      case TYPE_Plink:
        if (DEBUG) {
          std::cout << "P_link: " << h.trk_ID->ID << "->" << h.trk_link_ID->ID;
          std::cout << " [Score: " << h.probability << "]" << std::endl;
        }

        // push a link hypothesis
        m_links.push(h.trk_ID->ID, JoinHypothesis(h.trk_ID, h.trk_link_ID));
        break;


      // branch
      case TYPE_Pdivn:
        if (DEBUG) {
          std::cout << "P_branch: " << h.trk_ID->ID << "->" << h.trk_child_one_ID->ID;
          std::cout << " [Score: " << h.probability << "]" << std::endl;
          std::cout << "P_branch: " << h.trk_ID->ID << "->" << h.trk_child_two_ID->ID;
          std::cout << " [Score: " << h.probability << "]" << std::endl;
        }

        // push a branch hypothesis
        m_branches.push(h.trk_ID->ID, BranchHypothesis(h.trk_ID,
                                                       h.trk_child_one_ID,
                                                       h.trk_child_two_ID));
        break;


      case TYPE_Pmrge:
        // do nothing if we have a merge
        break;

    }
  }


  /* Merge the tracklets.

    i. Traverse the list of linkages.
    ii. Take the first tracklet, append subsequent objects to that tracklet,
        do not update object model
    iii. Rename subsequent tracklets
    iv. set the parent flags for the tracks
    v. Remove merged tracks

  */

  std::set<unsigned int> used;
  unsigned int child_j;

  // let's try to follow the links, iterate over the link hypotheses
  for (size_t parent_i=0; parent_i<m_links.size(); parent_i++) {

    // if we have a linkage, and we haven't already used this...
    if (!m_links[parent_i].empty() && used.count(parent_i)==0) {

      // now follow the chain
      used.emplace(parent_i);
      child_j = m_links[parent_i][0].second->ID;

      // merge the tracks
      if (DEBUG) std::cout << "Merge: [" << parent_i << ",";
      join_tracks(m_links[parent_i][0].first, m_links[parent_i][0].second);

      // mark the child as used
      used.emplace(child_j);

      // traverse the chain
      while(!m_links[child_j].empty()) {
        // merge the next track
        join_tracks(m_links[parent_i][0].first, m_links[child_j][0].second);

        // iterate
        child_j = m_links[child_j][0].second->ID;
        used.emplace(child_j);
      }
      if (DEBUG) std::cout << "]" << std::endl;
    }
  }


  // OK, now that we've merged all of the tracks, we want to set various flags
  // to show that divisions have occurred

  for (size_t parent_i=0; parent_i<m_branches.size(); parent_i++) {

    if (!m_branches[parent_i].empty()) {
      if (DEBUG) std::cout << "Branch: [";
      branch_tracks(m_branches[parent_i][0]);
      if (DEBUG) std::cout << "]" << std::endl;
    }

  }

  // erase those tracks marked for removal (i.e. those that have been merged)
  if (DEBUG) std::cout << "Tracks before merge: " << m_tracks.size();

  // remove the tracks if labelled to_remove
  m_tracks.erase( std::remove_if( m_tracks.begin(), m_tracks.end(),
                  [](const TrackletPtr &t) { return t->to_remove(); }),
                  m_tracks.end() );

  // give the user some more output
  if (DEBUG) std::cout << ", now " << m_tracks.size() << std::endl;

  // now finalise everything
  finalise();

}



void TrackManager::finalise()
{

  if (DEBUG) std::cout << "Finalising all tracks..." << std::endl;

  // set the global dummy ID counter here
  int dummy_ID = -1;
  m_dummies.clear();

  // iterate over the tracks, trim and renumber
  for (size_t i=0; i<m_tracks.size(); i++) {

    // first trim any tracks to remove any trailing dummy objects
    m_tracks[i]->trim();

    // now give the dummies unique IDs
    for (size_t o=0; o<m_tracks[i]->track.size(); o++) {
      if (m_tracks[i]->track[o]->dummy) {
        m_tracks[i]->track[o]->ID = dummy_ID;
        dummy_ID--;
        // add this dummy to the list
        m_dummies.push_back(m_tracks[i]->track[o]);
      }
    }
  }
}



TrackObjectPtr TrackManager::get_dummy(const int a_idx) const
{
  // first check that we're trying to get a dummy object (ID should be neg)
  assert(a_idx<0);
  assert(!m_dummies.empty());

  unsigned int dummy_idx = std::abs(a_idx+1);

  // sanity check that we've actually got a dummy
  assert(m_dummies[dummy_idx]->dummy);

  // return the dummies
  return m_dummies[dummy_idx];

}
