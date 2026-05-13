#include <iostream>
#include <string>
#include <limits>
#include <cctype>
#include <fstream>
#include <ctime>
#include <iomanip>
#include "class.h"
using namespace std;

// ── MAIN ─────────────────────────────────────────────────────────────────────

int main() {

    // ================= LOGIN =================

    if (!loginSystem()) {

        system("pause");
        return 0;
    }

    // ================= MAIN MENU =================

    while (true) {

        cout << "\n==========================================" << endl;
        cout << "      CRICKET MATCH SCORING SYSTEM        " << endl;
        cout << "==========================================" << endl;

        cout << "1. Start New Match" << endl;
        cout << "2. View Match History" << endl;
        cout << "3. Exit" << endl;

        cout << "\nEnter choice: ";

        int mainChoice = getValidInt();

        // ================= NEW MATCH =================

        if (mainChoice == 1) {

            string teamNameA, teamNameB;

            cout << "\nEnter Team A Name: ";
            teamNameA = getValidString();

            cout << "Enter Team B Name: ";
            teamNameB = getValidString();

            Team* teamA = new Team(teamNameA);
            Team* teamB = new Team(teamNameB);

            // TEAM A PLAYERS

            cout << "\nHow many players in "
                << teamNameA << "? ";

            int numPlayers = getValidInt();

            for (int i = 0; i < numPlayers; i++) {

                cout << "\nPlayer " << (i + 1) << ":\n";

                cout << "1. Batsman\n";
                cout << "2. Bowler\n";
                cout << "3. AllRounder\n";

                cout << "Enter type: ";

                int type = getValidInt();

                cout << "Enter player name: ";

                string name = getValidString();

                cout << "Enter jersey number: ";

                int jersey = getValidInt();

                if (type == 1)
                    teamA->addPlayer(new Batsman(name, jersey));

                else if (type == 2)
                    teamA->addPlayer(new Bowler(name, jersey));

                else
                    teamA->addPlayer(new AllRounder(name, jersey));
            }

            // TEAM B PLAYERS

            cout << "\nHow many players in "
                << teamNameB << "? ";

            numPlayers = getValidInt();

            for (int i = 0; i < numPlayers; i++) {

                cout << "\nPlayer " << (i + 1) << ":\n";

                cout << "1. Batsman\n";
                cout << "2. Bowler\n";
                cout << "3. AllRounder\n";

                cout << "Enter type: ";

                int type = getValidInt();

                cout << "Enter player name: ";

                string name = getValidString();

                cout << "Enter jersey number: ";

                int jersey = getValidInt();

                if (type == 1)
                    teamB->addPlayer(new Batsman(name, jersey));

                else if (type == 2)
                    teamB->addPlayer(new Bowler(name, jersey));

                else
                    teamB->addPlayer(new AllRounder(name, jersey));
            }

            // DISPLAY TEAMS

            teamA->displayTeam();
            teamB->displayTeam();

            // UMPIRE

            cout << "\nEnter Umpire Name: ";

            string umpireName = getValidString();

            Umpire* umpire = new Umpire(umpireName);

            cout << "\nUmpire "
                << umpire->getName()
                << " is officiating.\n";

            // MATCH

            cout << "\nEnter Match Name: ";

            string matchName = getValidString();

            Match* match =
                new Match(matchName, teamA, teamB);

            match->setUmpire(umpire);

            // GAME

            GameManager gm(match);

            gm.start();

            // CLEANUP

            delete match;
            delete umpire;
            delete teamA;
            delete teamB;
        }

        // ================= MATCH HISTORY =================

        else if (mainChoice == 2) {

            viewMatchHistory();
        }

        // ================= EXIT =================

        else if (mainChoice == 3) {

            cout << "\nExiting Program...\n";

            break;
        }

        else {

            cout << "\nInvalid Choice!\n";
        }
    }

    return 0;
}
