#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <algorithm>
#include <iomanip>
#include <cstdlib>

#ifdef _WIN32
    #include <windows.h>
    void clearScreen() {
        system("cls");
    }
#else
    void clearScreen() {
        system("clear");
    }
#endif

class Character {
private:
    std::string name;
    std::map<std::string, int> skills;
    int experience;
    int level;
    int activitiesLeft;
    int daysSinceActivity;
    static const int MAX_ACTIVITIES_PER_DAY = 3;
    static const int SKILL_DECAY_THRESHOLD = 7; // days

public:
    Character(const std::string& n) : name(n), experience(0), level(1), 
                                     activitiesLeft(MAX_ACTIVITIES_PER_DAY), daysSinceActivity(0) {
        // Initialize core meeting skills
        skills["Leadership"] = 1;
        skills["Communication"] = 1;
        skills["Problem_Solving"] = 1;
        skills["Teamwork"] = 1;
        skills["Presentation"] = 1;
    }

    const std::string& getName() const { return name; }
    int getLevel() const { return level; }
    int getExperience() const { return experience; }
    int getActivitiesLeft() const { return activitiesLeft; }
    int getDaysSinceActivity() const { return daysSinceActivity; }
    
    int getSkill(const std::string& skill) const {
        auto it = skills.find(skill);
        return (it != skills.end()) ? it->second : 0;
    }

    bool canDoActivity() const {
        return activitiesLeft > 0;
    }

    void useActivity() {
        if (activitiesLeft > 0) {
            activitiesLeft--;
            daysSinceActivity = 0;
        }
    }

    void newDay() {
        activitiesLeft = MAX_ACTIVITIES_PER_DAY;
        daysSinceActivity++;
        
        // Apply skill decay if inactive too long
        if (daysSinceActivity >= SKILL_DECAY_THRESHOLD) {
            applySkillDecay();
        }
    }

    void applySkillDecay() {
        std::cout << name << " has been inactive for " << daysSinceActivity 
                  << " days. Skills are decaying!\n";
        for (auto& skill : skills) {
            if (skill.second > 1) {
                skill.second--;
                std::cout << name << "'s " << skill.first << " decreased to " 
                          << skill.second << "\n";
            }
        }
    }

    void gainExperience(int exp) {
        experience += exp;
        int newLevel = 1 + (experience / 100);
        if (newLevel > level) {
            level = newLevel;
            std::cout << name << " leveled up to level " << level << "!\n";
        }
    }

    void improveSkill(const std::string& skill, int points) {
        skills[skill] += points;
        std::cout << name << "'s " << skill << " improved by " << points 
                  << " (now " << skills[skill] << ")\n";
    }

    void displayStats() const {
        std::cout << "\n=== " << name << " ===\n";
        std::cout << "Level: " << level << " | Experience: " << experience << "\n";
        std::cout << "Activities Left Today: " << activitiesLeft << "/" << MAX_ACTIVITIES_PER_DAY << "\n";
        std::cout << "Days Since Last Activity: " << daysSinceActivity << "\n";
        std::cout << "Skills:\n";
        for (const auto& skill : skills) {
            std::cout << "  " << std::setw(15) << skill.first << ": " << skill.second << "\n";
        }
    }
};

class MeetingTask {
public:
    std::string name;
    std::string description;
    std::string requiredSkill;
    int difficulty;
    int expReward;
    int skillReward;

    MeetingTask(const std::string& n, const std::string& desc, 
                const std::string& skill, int diff, int exp, int skillR)
        : name(n), description(desc), requiredSkill(skill), 
          difficulty(diff), expReward(exp), skillReward(skillR) {}
};

class MeetingGame {
private:
    std::vector<Character> characters;
    std::vector<MeetingTask> tasks;
    std::mt19937 rng;
    std::uniform_int_distribution<int> dice;
    int currentDay;

public:
    MeetingGame() : rng(std::random_device{}()), dice(1, 20), currentDay(1) {
        initializeTasks();
    }

    void initializeTasks() {
        tasks.emplace_back("Lead Discussion", "Guide the team through a complex topic", 
                          "Leadership", 10, 25, 2);
        tasks.emplace_back("Present Findings", "Share research results with the group", 
                          "Presentation", 8, 20, 2);
        tasks.emplace_back("Resolve Conflict", "Mediate between disagreeing team members", 
                          "Communication", 12, 30, 3);
        tasks.emplace_back("Brainstorm Solutions", "Generate creative ideas for challenges", 
                          "Problem_Solving", 6, 15, 1);
        tasks.emplace_back("Coordinate Tasks", "Organize team efforts and delegate work", 
                          "Teamwork", 9, 22, 2);
        tasks.emplace_back("Facilitate Workshop", "Run an interactive team building session", 
                          "Leadership", 15, 40, 3);
        tasks.emplace_back("Document Decisions", "Create clear meeting minutes and action items", 
                          "Communication", 5, 12, 1);
        tasks.emplace_back("Mentor Junior Member", "Help a new team member learn the ropes", 
                          "Teamwork", 7, 18, 2);
    }

    void addCharacter(const std::string& name) {
        characters.emplace_back(name);
        std::cout << name << " joined the meeting group!\n";
    }

    void displayCharacters() const {
        std::cout << "\n=== Team Members (Day " << currentDay << ") ===\n";
        for (size_t i = 0; i < characters.size(); ++i) {
            std::cout << i + 1 << ". " << characters[i].getName() 
                      << " (Level " << characters[i].getLevel() 
                      << ", Activities: " << characters[i].getActivitiesLeft() << "/3)\n";
        }
    }

    void displayTasks() const {
        std::cout << "\n=== Available Meeting Tasks ===\n";
        for (size_t i = 0; i < tasks.size(); ++i) {
            const auto& task = tasks[i];
            std::cout << i + 1 << ". " << task.name << "\n";
            std::cout << "   " << task.description << "\n";
            std::cout << "   Requires: " << task.requiredSkill 
                      << " (Difficulty: " << task.difficulty << ")\n";
            std::cout << "   Reward: " << task.expReward << " XP, +" 
                      << task.skillReward << " " << task.requiredSkill << "\n\n";
        }
    }

    std::vector<int> parseCharacterSelection(const std::string& input) {
        std::vector<int> indices;
        std::string current = "";
        
        for (char c : input + ",") {
            if (c == ',' || c == ' ') {
                if (!current.empty()) {
                    try {
                        int index = std::stoi(current) - 1;
                        if (index >= 0 && index < characters.size()) {
                            indices.push_back(index);
                        }
                    } catch (...) {}
                    current = "";
                }
            } else if (isdigit(c)) {
                current += c;
            }
        }
        
        // Remove duplicates
        std::sort(indices.begin(), indices.end());
        indices.erase(std::unique(indices.begin(), indices.end()), indices.end());
        
        return indices;
    }

    bool attemptTaskMultiple(const std::vector<int>& charIndices, int taskIndex) {
        if (taskIndex < 0 || taskIndex >= tasks.size()) {
            std::cout << "Invalid task selection!\n";
            return false;
        }

        if (charIndices.empty()) {
            std::cout << "No valid characters selected!\n";
            return false;
        }

        const MeetingTask& task = tasks[taskIndex];
        std::vector<Character*> availableChars;
        
        // Check which characters can participate
        for (int index : charIndices) {
            if (characters[index].canDoActivity()) {
                availableChars.push_back(&characters[index]);
            } else {
                std::cout << characters[index].getName() << " has no activities left today!\n";
            }
        }

        if (availableChars.empty()) {
            std::cout << "No characters available to do this activity!\n";
            return false;
        }

        std::cout << "\n=== Team Activity: " << task.name << " ===\n";
        std::cout << "Participants: ";
        for (size_t i = 0; i < availableChars.size(); ++i) {
            std::cout << availableChars[i]->getName();
            if (i < availableChars.size() - 1) std::cout << ", ";
        }
        std::cout << "\n\n";

        // Calculate team bonus (10% per additional member, max 50%)
        int teamBonus = std::min(4, static_cast<int>(availableChars.size() - 1)) * 2;
        if (teamBonus > 0) {
            std::cout << "Team Collaboration Bonus: +" << teamBonus << "\n";
        }

        bool anySuccess = false;
        
        // Each character attempts the task
        for (Character* character : availableChars) {
            int roll = dice(rng);
            int skillLevel = character->getSkill(task.requiredSkill);
            int totalScore = roll + skillLevel + teamBonus;

            std::cout << character->getName() << ": Roll " << roll 
                      << " + " << task.requiredSkill << "(" << skillLevel << ")";
            if (teamBonus > 0) std::cout << " + Team(" << teamBonus << ")";
            std::cout << " = " << totalScore << " vs " << task.difficulty << "\n";

            character->useActivity();

            if (totalScore >= task.difficulty) {
                std::cout << "  SUCCESS! ";
                character->gainExperience(task.expReward);
                character->improveSkill(task.requiredSkill, task.skillReward);
                anySuccess = true;
            } else {
                std::cout << "  FAILED! " << character->getName() 
                          << " gains " << (task.expReward / 3) << " XP for trying.\n";
                character->gainExperience(task.expReward / 3);
            }
        }

        // Additional team success bonus
        if (anySuccess && availableChars.size() > 1) {
            std::cout << "\nTeam activity bonus XP granted to all participants!\n";
            for (Character* character : availableChars) {
                character->gainExperience(5 * (availableChars.size() - 1));
            }
        }

        return anySuccess;
    }

    void nextDay() {
        currentDay++;
        clearScreen();
        std::cout << "=== Day " << currentDay << " begins! ===\n";
        
        for (auto& character : characters) {
            character.newDay();
        }
        
        std::cout << "All team members have refreshed their daily activities.\n";
        std::cout << "Press Enter to continue...";
        std::cin.ignore();
        std::cin.get();
    }

    void playRound() {
        clearScreen();
        if (characters.empty()) {
            std::cout << "No team members available! Add some first.\n";
            std::cout << "Press Enter to continue...";
            std::cin.ignore();
            std::cin.get();
            return;
        }

        displayCharacters();
        std::cout << "\nSelect team member(s) (e.g., '1' or '1,3,5' or '1 2 4'): ";
        std::string charInput;
        std::cin.ignore();
        std::getline(std::cin, charInput);
        
        std::vector<int> selectedChars = parseCharacterSelection(charInput);
        
        if (selectedChars.empty()) {
            std::cout << "No valid characters selected!\n";
            std::cout << "Press Enter to continue...";
            std::cin.get();
            return;
        }

        displayTasks();
        std::cout << "Select task (1-" << tasks.size() << "): ";
        int taskChoice;
        std::cin >> taskChoice;

        attemptTaskMultiple(selectedChars, taskChoice - 1);
        
        std::cout << "\nPress Enter to continue...";
        std::cin.ignore();
        std::cin.get();
    }

    void showStats() {
        clearScreen();
        if (characters.empty()) {
            std::cout << "No team members to display!\n";
            std::cout << "Press Enter to continue...";
            std::cin.ignore();
            std::cin.get();
            return;
        }
        
        for (const auto& character : characters) {
            character.displayStats();
        }
        
        std::cout << "\nPress Enter to continue...";
        std::cin.ignore();
        std::cin.get();
    }

    void runGame() {
        clearScreen();
        std::cout << "=== Welcome to SDEWG RPG ===\n";
        std::cout << "Build your team and level up through meeting challenges!\n";
        std::cout << "Each character can do 3 activities per day.\n";
        std::cout << "Inactive characters lose skills after 7 days!\n\n";
        std::cout << "Press Enter to start...";
        std::cin.get();

        while (true) {
            clearScreen();
            std::cout << "\n=== SDEWG RPG - Day " << currentDay << " ===\n";
            std::cout << "1. Add Team Member\n";
            std::cout << "2. Do Activity\n";
            std::cout << "3. View Team Stats\n";
            std::cout << "4. View Available Tasks\n";
            std::cout << "5. Next Day\n";
            std::cout << "6. Exit\n";
            std::cout << "Choice: ";

            int choice;
            std::cin >> choice;

            switch (choice) {
                case 1: {
                    clearScreen();
                    std::cout << "Enter team member name: ";
                    std::string name;
                    std::cin >> name;
                    addCharacter(name);
                    std::cout << "Press Enter to continue...";
                    std::cin.ignore();
                    std::cin.get();
                    break;
                }
                case 2:
                    playRound();
                    break;
                case 3:
                    showStats();
                    break;
                case 4:
                    clearScreen();
                    displayTasks();
                    std::cout << "Press Enter to continue...";
                    std::cin.ignore();
                    std::cin.get();
                    break;
                case 5:
                    nextDay();
                    break;
                case 6:
                    clearScreen();
                    std::cout << "Thanks for playing SDEWG RPG!\n";
                    return;
                default:
                    std::cout << "Invalid choice!\n";
            }
        }
    }
};

int main() {
    MeetingGame game;
    game.runGame();
    return 0;
}