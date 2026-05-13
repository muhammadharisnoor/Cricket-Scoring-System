#include <iostream>
#include <string>
#include <limits>
#include <cctype>
#include <fstream>
#include <ctime>
#include <iomanip>
using namespace std;

// ── Input helpers ────────────────────────────────────────────────────────────

int getValidInt() {
    int value;
    while (true) {
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // always flush
            return value;
        }
        else {
            cout << "Invalid input! Please enter a valid number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

string getValidString() {
    string input;
    while (true) {
        getline(cin, input);
        bool allWhitespace = true;
        for (char c : input) {
            if (!isspace(static_cast<unsigned char>(c))) {
                allWhitespace = false;
                break;
            }
        }
        if (!allWhitespace) return input;
        cout << "Input cannot be empty! Please enter a valid name: ";
    }
}

// ── Forward declarations ─────────────────────────────────────────────────────

class Match;
void savePlayerStats(class Team* team, string teamName);
void saveMatchHistory(Match* match);
void viewMatchHistory();

// ── CLASS 1: Player (Base) ───────────────────────────────────────────────────

class Player {
protected:
    string name;
    int    jerseyNo;

public:
    Player(string n, int jNo) : name(n), jerseyNo(jNo) {}

    string getName() { return name; }
    int    getJersey() { return jerseyNo; }

    virtual void displayStats() {
        cout << "Player: " << name << "  |  Jersey: " << jerseyNo << endl;
    }

    virtual ~Player() {}
};

// ── CLASS 2: Batsman ─────────────────────────────────────────────────────────

class Batsman : public Player {
private:
    int runs;
    int ballsFaced;

public:
    Batsman(string n, int jNo) : Player(n, jNo), runs(0), ballsFaced(0) {}

    void addRuns(int r) { runs += r; ballsFaced++; }

    int getRuns() { return runs; }
    int getBallsFaced() { return ballsFaced; }

    void displayStats() override {
        cout << "[BATSMAN]  " << name
            << "  | Jersey: " << jerseyNo
            << "  | Runs: " << runs
            << "  | Balls: " << ballsFaced;
        if (ballsFaced > 0)
            cout << "  | SR: " << fixed << setprecision(2)
            << (runs * 100.0 / ballsFaced);
        cout << endl;
    }
};

// ── CLASS 3: Bowler ──────────────────────────────────────────────────────────

class Bowler : public Player {
private:
    int wickets;
    int runsConceded;

public:
    Bowler(string n, int jNo) : Player(n, jNo), wickets(0), runsConceded(0) {}

    void addWicket() { wickets++; }
    void addRunsConceded(int r) { runsConceded += r; }

    int getWickets() { return wickets; }
    int getRunsConceded() { return runsConceded; }

    void displayStats() override {
        cout << "[BOWLER]   " << name
            << "  | Jersey: " << jerseyNo
            << "  | Wickets: " << wickets
            << "  | Runs Given: " << runsConceded
            << endl;
    }
};

// ── CLASS 4: AllRounder ──────────────────────────────────────────────────────

class AllRounder : public Player {
private:
    int runs, ballsFaced, wickets, runsConceded;

public:
    AllRounder(string n, int jNo)
        : Player(n, jNo), runs(0), ballsFaced(0), wickets(0), runsConceded(0) {
    }

    void addRuns(int r) { runs += r; ballsFaced++; }
    void addWicket() { wickets++; }
    void addRunsConceded(int r) { runsConceded += r; }

    int getRuns() { return runs; }
    int getBallsFaced() { return ballsFaced; }
    int getWickets() { return wickets; }
    int getRunsConceded() { return runsConceded; }

    void displayStats() override {
        cout << "[ALL-RNDR] " << name
            << "  | Jersey: " << jerseyNo
            << "  | Runs: " << runs
            << "  | Balls: " << ballsFaced
            << "  | Wkts: " << wickets
            << "  | Runs Given: " << runsConceded
            << endl;
    }
};

// ── CLASS 5: Ball ────────────────────────────────────────────────────────────

class Ball {
private:
    int    ballNo;
    int    runs;
    bool   isWicket;
    bool   isExtra;
    string extraType;

public:
    Ball() : ballNo(0), runs(0), isWicket(false), isExtra(false), extraType("") {}

    void setBallNo(int n) { ballNo = n; }
    void setRuns(int r) { runs = r; }
    void setWicket() { isWicket = true; }
    void setExtra(string type, int r) { isExtra = true; extraType = type; runs = r; }

    int    getRuns() { return runs; }
    bool   getIsWicket() { return isWicket; }
    bool   getIsExtra() { return isExtra; }
    string getExtraType() { return extraType; }
    int    getBallNo() { return ballNo; }
};

// ── CLASS 6: Over ────────────────────────────────────────────────────────────

class Over {
private:
    int  overNo;
    Ball balls[6];
    int  ballCount;
    int  runsInOver;

public:
    Over() : overNo(0), ballCount(0), runsInOver(0) {}

    void setOverNo(int n) { overNo = n; }
    int  getOverNo() { return overNo; }

    bool isComplete() { return ballCount >= 6; }
    int  getBallCount() { return ballCount; }
    int  getRunsInOver() { return runsInOver; }

    void addBall(Ball b) {
        if (ballCount < 6) {
            balls[ballCount] = b;
            runsInOver += b.getRuns();
            ballCount++;
        }
    }
};

// ── CLASS 7: Innings ─────────────────────────────────────────────────────────

class Innings {
private:
    int   inningNo;
    Over  overs[50];
    int   overCount;
    int   totalRuns;
    int   totalWickets;

public:
    Innings() : inningNo(0), overCount(0), totalRuns(0), totalWickets(0) {}

    void setInningNo(int n) { inningNo = n; }
    int  getInningNo() { return inningNo; }

    void addRuns(int r) { totalRuns += r; }
    void addWicket() { totalWickets++; }

    void addOver() {
        if (overCount < 50) {
            overs[overCount].setOverNo(overCount + 1);
            overCount++;
        }
    }

    void addBallToCurrentOver(Ball b) {
        if (overCount > 0)
            overs[overCount - 1].addBall(b);
    }

    bool isCurrentOverComplete() {
        if (overCount == 0) return true;
        return overs[overCount - 1].isComplete();
    }

    float getRunRate() {
        if (overCount == 0) return 0.0f;
        return static_cast<float>(totalRuns) / static_cast<float>(overCount);
    }

    int getTotalRuns() { return totalRuns; }
    int getTotalWickets() { return totalWickets; }
    int getOverCount() { return overCount; }

    int getBallsInCurrentOver() {
        if (overCount == 0) return 0;
        return overs[overCount - 1].getBallCount();
    }

    void displayInnings() {
        cout << "Innings " << inningNo << " => "
            << totalRuns << "/" << totalWickets
            << "  (" << overCount << " overs)" << endl;
    }
};

// ── CLASS 8: Team ────────────────────────────────────────────────────────────

class Team {
private:
    string  teamName;
    Player* players[15];
    int     playerCount;

public:
    Team(string name) : teamName(name), playerCount(0) {
        for (int i = 0; i < 15; i++) players[i] = nullptr;
    }

    ~Team() {
        for (int i = 0; i < playerCount; i++) {
            delete players[i];
            players[i] = nullptr;
        }
    }

    string getName() { return teamName; }
    int    getPlayerCount() { return playerCount; }

    void addPlayer(Player* p) {
        if (playerCount < 15)
            players[playerCount++] = p;
        else
            cout << "Team is full! Cannot add more players." << endl;
    }

    Player* getPlayer(int index) {
        if (index >= 0 && index < playerCount) return players[index];
        return nullptr;
    }

    void displayTeam() {
        cout << "\n===== TEAM: " << teamName << " =====" << endl;
        for (int i = 0; i < playerCount; i++) {
            cout << (i + 1) << ". ";
            players[i]->displayStats();
        }
    }
};

// ── CLASS 9: Match ───────────────────────────────────────────────────────────

class Umpire; // forward for Match

class Match {
private:
    string   matchName;
    Team* teamA;
    Team* teamB;
    int      currentInnings;
    string   winner;
    Innings  innings[2];
    Umpire* umpire;

public:
    Match(string name, Team* a, Team* b)
        : matchName(name), teamA(a), teamB(b),
        currentInnings(1), winner("TBD"), umpire(nullptr) {
        innings[0].setInningNo(1);
        innings[1].setInningNo(2);
    }

    string  getMatchName() { return matchName; }
    Team* getTeamA() { return teamA; }
    Team* getTeamB() { return teamB; }
    int     getCurrentInnings() { return currentInnings; }
    string  getWinner() { return winner; }

    void setUmpire(Umpire* u) { umpire = u; }

    Innings* getCurrentInningsObj() { return &innings[currentInnings - 1]; }
    Innings* getInnings(int n) {
        if (n == 1 || n == 2) return &innings[n - 1];
        return nullptr;
    }

    void startMatch() {
        cout << "\n************************************" << endl;
        cout << "   MATCH STARTED: " << matchName << endl;
        cout << "   " << teamA->getName() << "  vs  " << teamB->getName() << endl;
        cout << "************************************" << endl;
    }

    void nextInnings() {
        if (currentInnings == 1) {
            currentInnings = 2;
            cout << "\n--- END OF INNINGS 1 ---" << endl;
            cout << "--- INNINGS 2 BEGINS ---\n" << endl;
        }
        else {
            cout << "Match is already in the 2nd innings!" << endl;
        }
    }

    void setWinner(string winnerName) {
        winner = winnerName;
        cout << "\n*** MATCH RESULT: " << winner << " WINS! ***" << endl;
    }

    void declareResult() {
        int runs1 = innings[0].getTotalRuns();
        int runs2 = innings[1].getTotalRuns();
        if (runs1 > runs2) setWinner(teamA->getName());
        else if (runs2 > runs1) setWinner(teamB->getName());
        else {
            winner = "DRAW";
            cout << "\n*** MATCH RESULT: It's a DRAW! ***" << endl;
        }
    }
};

// ── CLASS 10: Umpire ─────────────────────────────────────────────────────────

class Umpire {
private:
    string name;

public:
    Umpire(string n) : name(n) {}

    string getName() { return name; }

    bool isOut(string reason) {
        cout << "[UMPIRE " << name << "] Decision: OUT! (" << reason << ")" << endl;
        return true;
    }
};

// ── CLASS 11: Scoreboard ─────────────────────────────────────────────────────

class Scoreboard {
private:
    Match* match;

public:
    Scoreboard(Match* m) : match(m) {}

    void displayScore() {
        cout << "\n========== SCOREBOARD ==========" << endl;
        cout << "Match: " << match->getMatchName() << endl;
        cout << "Current Innings: " << match->getCurrentInnings() << endl;

        Innings* cur = match->getCurrentInningsObj();
        cout << "Score: " << cur->getTotalRuns()
            << "/" << cur->getTotalWickets() << endl;
        cout << "Overs: " << cur->getOverCount()
            << "." << cur->getBallsInCurrentOver() << endl;
        if (cur->getOverCount() > 0)
            cout << "Run Rate: " << fixed << setprecision(2)
            << cur->getRunRate() << endl;
        cout << "================================" << endl;
    }

    void displayBattingStats() {
        cout << "\n--- Batting Stats (Current Innings) ---" << endl;
        Team* batting = (match->getCurrentInnings() == 1)
            ? match->getTeamA() : match->getTeamB();
        for (int i = 0; i < batting->getPlayerCount(); i++)
            batting->getPlayer(i)->displayStats();
    }

    void displayBowlingStats() {
        cout << "\n--- Bowling Stats (Current Innings) ---" << endl;
        Team* bowling = (match->getCurrentInnings() == 1)
            ? match->getTeamB() : match->getTeamA();
        for (int i = 0; i < bowling->getPlayerCount(); i++)
            bowling->getPlayer(i)->displayStats();
    }

    void displaySummary() {
        cout << "\n========== MATCH SUMMARY ==========" << endl;
        cout << "Match:  " << match->getMatchName() << endl;
        cout << "Team A: " << match->getTeamA()->getName() << endl;
        cout << "Team B: " << match->getTeamB()->getName() << endl;

        Innings* inn1 = match->getInnings(1);
        Innings* inn2 = match->getInnings(2);

        cout << "\nInnings 1: "; inn1->displayInnings();
        cout << "Innings 2: "; inn2->displayInnings();

        cout << "\nWINNER: " << match->getWinner() << endl;
        cout << "====================================" << endl;
    }
};

// ── File I/O helpers ─────────────────────────────────────────────────────────

void savePlayerStats(Team* team, string teamName) {
    ofstream file("player_stats.csv", ios::app);
    if (!file) { cout << "Error opening player_stats.csv\n"; return; }

    for (int i = 0; i < team->getPlayerCount(); i++) {
        Player* p = team->getPlayer(i);
        Batsman* ba = dynamic_cast<Batsman*>(p);
        Bowler* bo = dynamic_cast<Bowler*>(p);
        AllRounder* ar = dynamic_cast<AllRounder*>(p);

        if (ba) {
            float sr = (ba->getBallsFaced() > 0)
                ? (ba->getRuns() * 100.0f / ba->getBallsFaced()) : 0.0f;
            file << ba->getName() << "," << teamName << ","
                << ba->getRuns() << "," << ba->getBallsFaced() << ","
                << fixed << setprecision(2) << sr << ",0,0\n";
        }
        else if (bo) {
            file << bo->getName() << "," << teamName
                << ",0,0,0," << bo->getWickets() << ","
                << bo->getRunsConceded() << "\n";
        }
        else if (ar) {
            float sr = (ar->getBallsFaced() > 0)
                ? (ar->getRuns() * 100.0f / ar->getBallsFaced()) : 0.0f;
            file << ar->getName() << "," << teamName << ","
                << ar->getRuns() << "," << ar->getBallsFaced() << ","
                << fixed << setprecision(2) << sr << ","
                << ar->getWickets() << "," << ar->getRunsConceded() << "\n";
        }
    }
    file.close();
}

void saveMatchHistory(Match* match) {

    ofstream file("match_results.csv", ios::app);

    if (!file) {
        cout << "Error opening match_results.csv\n";
        return;
    }

    time_t now = time(nullptr);

    char dt[26];

    ctime_s(dt, sizeof(dt), &now);

    string dateTime = dt;

    if (!dateTime.empty() && dateTime.back() == '\n')
        dateTime.pop_back();

    Innings* inn1 = match->getInnings(1);
    Innings* inn2 = match->getInnings(2);

    file << match->getMatchName() << ","
        << match->getTeamA()->getName() << ","
        << inn1->getTotalRuns() << "/"
        << inn1->getTotalWickets() << ","
        << match->getTeamB()->getName() << ","
        << inn2->getTotalRuns() << "/"
        << inn2->getTotalWickets() << ","
        << match->getWinner() << ","
        << dateTime << "\n";

    file.close();
}

void viewMatchHistory() {
    ifstream file("match_results.csv");
    if (!file) { cout << "\nNo match history found.\n"; return; }

    string line;
    cout << "\n========== PREVIOUS MATCH HISTORY ==========\n";
    while (getline(file, line)) cout << line << "\n";
    cout << "============================================\n";
    file.close();
}

// ── CLASS 12: GameManager ────────────────────────────────────────────────────

class GameManager {
private:
    Match* match;

public:
    GameManager(Match* m) : match(m) {}

    void showMenu() {
        cout << "\n====== CRICKET SCORING MENU ======" << endl;
        cout << "1. Add Runs" << endl;
        cout << "2. Add Wicket" << endl;
        cout << "3. Add Over" << endl;
        cout << "4. Display Score" << endl;
        cout << "5. Switch to Next Innings" << endl;
        cout << "6. Show Player Stats (Team A)" << endl;
        cout << "7. Show Player Stats (Team B)" << endl;
        cout << "8. View Previous Match History" << endl;
        cout << "9. Declare Result & Exit" << endl;
        cout << "==================================" << endl;
        cout << "Your choice: ";
    }

    void handleUserChoice(int choice) {
        Innings* cur = match->getCurrentInningsObj();
        Scoreboard  board(match);

        switch (choice) {
        case 1: {
            cout << "Enter runs scored on this ball: ";
            int r = getValidInt();
            if (cur->isCurrentOverComplete()) {
                cur->addOver();
                cout << "[New over started]" << endl;
            }
            Ball b;
            b.setRuns(r);
            cur->addBallToCurrentOver(b);
            cur->addRuns(r);
            cout << "Runs added: " << r << endl;
            break;
        }
        case 2:
            cur->addWicket();
            cout << "Wicket taken! Total wickets: "
                << cur->getTotalWickets() << endl;
            break;
        case 3:
            cur->addOver();
            cout << "Over " << cur->getOverCount() << " started." << endl;
            break;
        case 4:
            board.displayScore();
            break;
        case 5:
            match->nextInnings();
            break;
        case 6:
            match->getTeamA()->displayTeam();
            break;
        case 7:
            match->getTeamB()->displayTeam();
            break;
        case 8:
            viewMatchHistory();
            break;
        case 9:
            match->declareResult();
            savePlayerStats(match->getTeamA(), match->getTeamA()->getName());
            savePlayerStats(match->getTeamB(), match->getTeamB()->getName());
            saveMatchHistory(match);
            board.displaySummary();
            cout << "\nData saved successfully!\n";
            break;
        default:
            cout << "Invalid choice! Please try again." << endl;
        }
    }

    void start() {
        match->startMatch();
        int choice = 0;
        while (choice != 9) {
            showMenu();
            choice = getValidInt();
            handleUserChoice(choice);
        }
    }
};

// ================= LOGIN SYSTEM =================

bool loginSystem() {

    string correctUsername = "admin";
    string correctPassword = "admin";

    string username;
    string password;

    int attempts = 3;

    while (attempts > 0) {

        cout << "\n==================================" << endl;
        cout << "         RUN SCORER LOGIN         " << endl;
        cout << "==================================" << endl;

        cout << "Username: ";
        username = getValidString();

        cout << "Password: ";
        password = getValidString();

        if (username == correctUsername &&
            password == correctPassword) {

            cout << "\nLogin Successful!\n";

            return true;
        }

        else {

            attempts--;

            cout << "\nIncorrect Username or Password!" << endl;

            if (attempts > 0) {

                cout << "Attempts Remaining: "
                    << attempts << endl;
            }
        }
    }

    cout << "\nToo many failed attempts!" << endl;

    return false;
}
