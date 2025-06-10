#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <algorithm>
#include <iomanip>

class Character {
private:
    std::string name;
    std::map<std::string, int> skills;
    int experience;
    int level;

public:
    Character(const std::string& n) : name(n), experience(0), level(1) {
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
    
    int getSkill(const std::string& skill) const {
        auto it = skills.find(skill);
        return (it != skills.end()) ? it->second : 0;
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

public:
    MeetingGame() : rng(std::random_device{}()), dice(1, 20) {
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
        std::cout << "\n=== Team Members ===\n";
        for (size_t i = 0; i < characters.size(); ++i) {
            std::cout << i + 1 << ". " << characters[i].getName() 
                      << " (Level " << characters[i].getLevel() << ")\n";
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

    bool attemptTask(int charIndex, int taskIndex) {
        if (charIndex < 0 || charIndex >= characters.size() || 
            taskIndex < 0 || taskIndex >= tasks.size()) {
            std::cout << "Invalid selection!\n";
            return false;
        }

        Character& character = characters[charIndex];
        const MeetingTask& task = tasks[taskIndex];

        std::cout << "\n" << character.getName() << " attempts: " << task.name << "\n";
        
        // Roll dice + skill level vs difficulty
        int roll = dice(rng);
        int skillLevel = character.getSkill(task.requiredSkill);
        int totalScore = roll + skillLevel;

        std::cout << "Roll: " << roll << " + " << task.requiredSkill 
                  << " (" << skillLevel << ") = " << totalScore 
                  << " vs Difficulty " << task.difficulty << "\n";

        if (totalScore >= task.difficulty) {
            std::cout << "SUCCESS! ";
            character.gainExperience(task.expReward);
            character.improveSkill(task.requiredSkill, task.skillReward);
            return true;
        } else {
            std::cout << "FAILED! " << character.getName() 
                      << " gains " << (task.expReward / 3) << " XP for trying.\n";
            character.gainExperience(task.expReward / 3);
            return false;
        }
    }

    void playRound() {
        if (characters.empty()) {
            std::cout << "No team members available! Add some first.\n";
            return;
        }

        displayCharacters();
        std::cout << "\nSelect team member (1-" << characters.size() << "): ";
        int charChoice;
        std::cin >> charChoice;

        displayTasks();
        std::cout << "Select task (1-" << tasks.size() << "): ";
        int taskChoice;
        std::cin >> taskChoice;

        attemptTask(charChoice - 1, taskChoice - 1);
    }

    void showStats() {
        if (characters.empty()) {
            std::cout << "No team members to display!\n";
            return;
        }
        
        for (const auto& character : characters) {
            character.displayStats();
        }
    }

    void runGame() {
        std::cout << "=== Welcome to Meeting Masters RPG ===\n";
        std::cout << "Build your team and level up through meeting challenges!\n\n";

        while (true) {
            std::cout << "\n=== Main Menu ===\n";
            std::cout << "1. Add Team Member\n";
            std::cout << "2. Play Round\n";
            std::cout << "3. View Team Stats\n";
            std::cout << "4. View Available Tasks\n";
            std::cout << "5. Exit\n";
            std::cout << "Choice: ";

            int choice;
            std::cin >> choice;

            switch (choice) {
                case 1: {
                    std::cout << "Enter team member name: ";
                    std::string name;
                    std::cin >> name;
                    addCharacter(name);
                    break;
                }
                case 2:
                    playRound();
                    break;
                case 3:
                    showStats();
                    break;
                case 4:
                    displayTasks();
                    break;
                case 5:
                    std::cout << "Thanks for playing Meeting Masters RPG!\n";
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