#include "RecursionToTheRescue.h"
#include "map.h"
#include <climits>
#include <iostream>
#include "Disasters.h"
#include "grid.h"
using namespace std;

/* * * * Doctors Without Orders * * * */

/**
 * Given a list of doctors and a list of patients, determines whether all the patients can
 * be seen. If so, this function fills in the schedule outparameter with a map from doctors
 * to the set of patients that doctor would see.
 *
 * @param doctors  The list of the doctors available to work.
 * @param patients The list of the patients that need to be seen.
 * @param schedule An outparameter that will be filled in with the schedule, should one exist.
 * @return Whether or not a schedule was found.
 */


void canAllPatientsBeSeenHelper(const Vector<Doctor>& doctors,
                                Vector<Patient>& patients,
                                Map<string, Set<string>>& schedule,
                                Vector<int>& patientsTime,
                                bool& isFound,
                                Map<string, Set<string>>& finalSchedule) {
    // base case: all the patients been seen, return true
    if (isFound) return;
    if (patients.isEmpty()) {
        bool isVaLid = true;
        int n = doctors.size();
        for (int i = 0; i < n; i++) {
            if (doctors[i].hoursFree < patientsTime[i]) {
                isVaLid = false;
            }
        }
        if (isVaLid) {
            isFound = true;
            finalSchedule = schedule;
        }
    } else {
        // put the first patient in the schedule, consider every doctor
        Patient p = patients[0];
        patients.remove(0);
        int n = doctors.size();

        // loop the schedule, each doctor
        for (int i = 0; i < n; i++) {
            // confirm if the patients can be seen
            if (doctors[i].hoursFree >= patientsTime[i] + p.hoursNeeded) {

                patientsTime[i] += p.hoursNeeded;
                schedule[doctors[i].name].add(p.name);

                canAllPatientsBeSeenHelper(doctors, patients, schedule, patientsTime, isFound, finalSchedule);

                patientsTime[i] -= p.hoursNeeded;
                schedule[doctors[i].name].remove(p.name);
            }
        }
        // add the patient back to the vector
        patients.insert(0, p);
    }
}

bool canAllPatientsBeSeen(const Vector<Doctor>& doctors,
                          const Vector<Patient>& patients,
                          Map<string, Set<string>>& schedule) {

    Vector<int> patientsTime;
    Map<string, Set<string>> scheduleTest;
    int n = doctors.size();
    for (int i = 0; i < n; i++) {
        scheduleTest[doctors[i].name] = {};
        patientsTime.add(0);
    }
    bool isFound = false;
    Vector<Patient> patientsCopy = patients;
    canAllPatientsBeSeenHelper(doctors, patientsCopy, scheduleTest, patientsTime, isFound, schedule);
    return isFound;
}

/* * * * Disaster Planning * * * */

/**
 * Given a transportation grid for a country or region, along with the number of cities where disaster
 * supplies can be stockpiled, returns whether it's possible to stockpile disaster supplies in at most
 * the specified number of cities such that each city either has supplies or is connected to a city that
 * does.
 * <p>
 * This function can assume that every city is a key in the transportation map and that roads are
 * bidirectional: if there's a road from City A to City B, then there's a road from City B back to
 * City A as well.
 *
 * @param roadNetwork The underlying transportation network.
 * @param numCities   How many cities you can afford to put supplies in.
 * @param locations   An outparameter filled in with which cities to choose if a solution exists.
 * @return Whether a solution exists.
 */

void canBeMadeDisasterReadyHelper(const Map<string, Set<string>>& roadNetwork,
                                  int numCities,
                                  Set<string>& finalLocations,
                                  Set<string> retainLocations,
                                  Set<string>& locations,
                                  bool& isFound) {
    // if any solution is found
    if (isFound) return;

    // if there's no location retained
    if (retainLocations.isEmpty()) {
        isFound = true;
        finalLocations = locations;
        return;
    } else {
        if (numCities == 0) return;
        // choose a uncovered city
        string uncoverCity = retainLocations.first();
        Set<string> uncoverCities = roadNetwork[uncoverCity];
        uncoverCities.add(uncoverCity);
        for (string currentLocation : uncoverCities) {
            Set<string> currentNetwork = roadNetwork[currentLocation];
            currentNetwork.add(currentLocation);
            Set<string> newRetainLocations = retainLocations - currentNetwork;
            locations += currentLocation;
            canBeMadeDisasterReadyHelper(roadNetwork, numCities - 1, finalLocations, newRetainLocations, locations, isFound);
            locations -= currentLocation;
        }
    }
}

bool canBeMadeDisasterReady(const Map<string, Set<string>>& roadNetwork,
                            int numCities,
                            Set<string>& locations) {
    bool isFound = false;
    Set<string> retainLocations;
    Set<string> chosenLocations;
    for (string city : roadNetwork) {
        retainLocations.add(city);
    }
    canBeMadeDisasterReadyHelper(roadNetwork, numCities, locations, retainLocations, chosenLocations, isFound);
    return isFound;
}

/* * * * Winning the Election * * * */

/**
 * Given a list of the states in the election, including their popular and electoral vote
 * totals, and the number of electoral votes needed, as well as the index of the lowest-indexed
 * state to consider, returns information about how few popular votes you'd need in order to
 * win that at least that many electoral votes.
 *
 * @param electoralVotesNeeded the minimum number of electoral votes needed
 * @param states All the states in the election (plus DC, if appropriate)
 * @param minStateIndex the lowest index in the states Vector that should be considered
 * @param minInfoGrid Save the result that has been calculated
 */
MinInfo minPopularVoteToGetAtLeast(int electoralVotesNeeded,
                                   const Vector<State>& states,
                                   int minStateIndex,
                                   Grid<MinInfo>& minInfoGrid) {
    // the fundamental case
    if (electoralVotesNeeded <= 0) {
        return {0, {}};
    }

    // if the result was saved
    if (minInfoGrid[electoralVotesNeeded - 1][minStateIndex].popularVotesNeeded != -1) {
        return minInfoGrid[electoralVotesNeeded - 1][minStateIndex];
    }

    // get the current information
    State currentState = states[minStateIndex];
    int currentPopularVoteNeeded = (currentState.popularVotes / 2) + 1;
    int currentElectoralVoteNeeded = currentState.electoralVotes;

    // the lase state, base case
    if (minStateIndex == states.size() - 1) {
        if (electoralVotesNeeded > currentElectoralVoteNeeded) {
            minInfoGrid[electoralVotesNeeded - 1][minStateIndex] = {INT_MAX, {}};
            return {INT_MAX, {}};
        } else {
            MinInfo lastInfo = {currentPopularVoteNeeded, {states[minStateIndex]}};
            minInfoGrid[electoralVotesNeeded - 1][minStateIndex] = lastInfo;
            return lastInfo;
        }
    }

    // put this state into the result
    int newElectoralVotesNeeded = electoralVotesNeeded - currentElectoralVoteNeeded;
    MinInfo nextWithCurrent = minPopularVoteToGetAtLeast(newElectoralVotesNeeded, states, minStateIndex + 1, minInfoGrid);

    // put this state not into the result
    MinInfo nextWithoutCurrent = minPopularVoteToGetAtLeast(electoralVotesNeeded, states, minStateIndex + 1, minInfoGrid);

    if (nextWithCurrent.popularVotesNeeded == INT_MAX) {
        minInfoGrid[electoralVotesNeeded - 1][minStateIndex] = nextWithoutCurrent;
        return nextWithoutCurrent;
    }

    nextWithCurrent.popularVotesNeeded += currentPopularVoteNeeded;
    nextWithCurrent.statesUsed.insert(0, currentState);

    // compare and return the result
    if (nextWithCurrent.popularVotesNeeded > nextWithoutCurrent.popularVotesNeeded) {
        minInfoGrid[electoralVotesNeeded - 1][minStateIndex] = nextWithoutCurrent;
        return nextWithoutCurrent;
    } else {
        minInfoGrid[electoralVotesNeeded - 1][minStateIndex] = nextWithCurrent;
        return nextWithCurrent;
    }
}

/**
 * Given a list of all the states in an election, including their popular and electoral vote
 * totals, returns information about how few popular votes you'd need to win in order to win
 * the presidency.
 *
 * @param states All the states in the election (plus DC, if appropriate)
 * @return Information about how few votes you'd need to win the election.
 */
MinInfo minPopularVoteToWin(const Vector<State>& states) {

    int electoralVotesSum = 0;

    for (State state : states) {
        electoralVotesSum += state.electoralVotes;
    }

    int electoralVotesNeeded = (electoralVotesSum / 2) + 1;

    // the grid we used in the helper function to memopization
    Grid<MinInfo> minInfoGrid (electoralVotesNeeded, states.size());

    // initialize the grid with {-1, {}}, distinguish with {-1, {}}
    minInfoGrid.fill({-1, {}});

    // calculate the sentinel values of each i
    Vector<int> sentinelValues;
    for (int i = 0; i < states.size(); i++) {
        int currentSentinelValues = 0;
        for (int j = i; j < states.size(); j++) {
            currentSentinelValues += states[j].electoralVotes;
        }
        sentinelValues.add(currentSentinelValues);
    }

    // if the v and i is higher than the sentinel value, reassign it as INT_MAX
    for (int i = 0; i < electoralVotesNeeded; i++) {
        for (int j = 0; j < states.size(); j++) {
            if (i > sentinelValues[j]) {
                minInfoGrid[i][j] = {INT_MAX, {}};
            }
        }
    }

    return minPopularVoteToGetAtLeast(electoralVotesNeeded, states, 0, minInfoGrid);
}
