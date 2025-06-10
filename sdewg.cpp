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

enum class JobLevel {
    INTERN = 0,
    ENGINEER_1 = 1,
    ENGINEER_2 = 2,
    SENIOR_ENGINEER = 3,
    PRINCIPAL_ENGINEER = 4,
    DISTINGUISHED_ENGINEER = 5,
    FELLOW = 6
};

class Character {
private:
    std::string name;
    std::map<std::string, int> skills;
    int experience;
    int level;
    JobLevel jobLevel;
    bool eligibleForPromotion;
    int activitiesLeft;
    int daysSinceActivity;
    static const int MAX_ACTIVITIES_PER_DAY = 3;
    static const int SKILL_DECAY_THRESHOLD = 7; // days

    std::string getJobLevelName(JobLevel jl) const {
        switch (jl) {
            case JobLevel::INTERN: return "Intern";
            case JobLevel::ENGINEER_1: return "Engineer 1";
            case JobLevel::ENGINEER_2: return "Engineer 2";
            case JobLevel::SENIOR_ENGINEER: return "Senior Engineer";
            case JobLevel::PRINCIPAL_ENGINEER: return "Principal Engineer";
            case JobLevel::DISTINGUISHED_ENGINEER: return "Distinguished Engineer";
            case JobLevel::FELLOW: return "Fellow";
            default: return "Unknown";
        }
    }

    int getPromotionRequirement(JobLevel jl) const {
        switch (jl) {
            case JobLevel::INTERN: return 200;           // To Engineer 1
            case JobLevel::ENGINEER_1: return 500;       // To Engineer 2
            case JobLevel::ENGINEER_2: return 1000;      // To Senior Engineer
            case JobLevel::SENIOR_ENGINEER: return 2000; // To Principal Engineer
            case JobLevel::PRINCIPAL_ENGINEER: return 4000; // To Distinguished Engineer
            case JobLevel::DISTINGUISHED_ENGINEER: return 8000; // To Fellow
            default: return 999999; // Fellow is max level
        }
    }

public:
    Character(const std::string& n) : name(n), experience(0), level(1), 
                                     jobLevel(JobLevel::INTERN), eligibleForPromotion(false),
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
    JobLevel getJobLevel() const { return jobLevel; }
    std::string getJobLevelString() const { return getJobLevelName(jobLevel); }
    bool isEligibleForPromotion() const { return eligibleForPromotion; }
    
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

    void checkPromotionEligibility() {
        if (jobLevel == JobLevel::FELLOW) return; // Max level reached
        
        int requiredExp = getPromotionRequirement(jobLevel);
        if (experience >= requiredExp && !eligibleForPromotion) {
            eligibleForPromotion = true;
            std::cout << "\n*** " << name << " is eligible for promotion to " 
                      << getJobLevelName(static_cast<JobLevel>(static_cast<int>(jobLevel) + 1)) 
                      << "! ***\n";
            std::cout << "Complete a promotion task to advance!\n";
        }
    }

    bool attemptPromotion() {
        if (!eligibleForPromotion || jobLevel == JobLevel::FELLOW) {
            return false;
        }
        
        jobLevel = static_cast<JobLevel>(static_cast<int>(jobLevel) + 1);
        eligibleForPromotion = false;
        
        std::cout << "\n🎉 PROMOTION! " << name << " is now a " 
                  << getJobLevelName(jobLevel) << "! 🎉\n";
        
        // Promotion bonus
        gainExperience(100);
        for (auto& skill : skills) {
            skill.second += 1;
        }
        std::cout << "Promotion bonus: +100 XP and +1 to all skills!\n";
        
        return true;
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
        checkPromotionEligibility();
    }

    void improveSkill(const std::string& skill, int points) {
        skills[skill] += points;
        std::cout << name << "'s " << skill << " improved by " << points 
                  << " (now " << skills[skill] << ")\n";
    }

    void displayStats() const {
        std::cout << "\n=== " << name << " ===\n";
        std::cout << "Job Level: " << getJobLevelName(jobLevel);
        if (eligibleForPromotion) {
            std::cout << " (PROMOTION READY!)";
        }
        std::cout << "\n";
        std::cout << "Level: " << level << " | Experience: " << experience;
        if (jobLevel != JobLevel::FELLOW) {
            int nextPromo = getPromotionRequirement(jobLevel);
            std::cout << " (Next promotion: " << nextPromo << ")";
        }
        std::cout << "\n";
        std::cout << "Activities Left Today: " << activitiesLeft << "/" << MAX_ACTIVITIES_PER_DAY << "\n";
        std::cout << "Days Since Last Activity: " << daysSinceActivity << "\n";
        std::cout << "Skills:\n";
        for (const auto& skill : skills) {
            std::cout << "  " << std::setw(15) << skill.first << ": " << skill.second << "\n";
        }
    }
};

class PromotionTask {
public:
    std::string name;
    std::string description;
    JobLevel requiredLevel;
    std::map<std::string, int> skillRequirements;
    int difficulty;

    PromotionTask(const std::string& n, const std::string& desc, JobLevel level, 
                  const std::map<std::string, int>& skillReqs, int diff)
        : name(n), description(desc), requiredLevel(level), 
          skillRequirements(skillReqs), difficulty(diff) {}
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
    std::vector<PromotionTask> promotionTasks;
    std::mt19937 rng;
    std::uniform_int_distribution<int> dice;
    int currentDay;

public:
    MeetingGame() : rng(std::random_device{}()), dice(1, 20), currentDay(1) {
        initializeTasks();
        initializePromotionTasks();
    }

    void initializePromotionTasks() {
        // Intern -> Engineer 1
        promotionTasks.emplace_back("Complete First Project", 
            "Successfully deliver your first major project contribution",
            JobLevel::INTERN, 
            std::map<std::string, int>{{"Communication", 3}, {"Teamwork", 3}}, 15);
        
        // Engineer 1 -> Engineer 2
        promotionTasks.emplace_back("Lead Technical Initiative", 
            "Take ownership of a technical solution and guide its implementation",
            JobLevel::ENGINEER_1, 
            std::map<std::string, int>{{"Problem_Solving", 5}, {"Leadership", 4}}, 18);
        
        // Engineer 2 -> Senior Engineer
        promotionTasks.emplace_back("Mentor Junior Engineers", 
            "Successfully guide and develop junior team members",
            JobLevel::ENGINEER_2, 
            std::map<std::string, int>{{"Leadership", 6}, {"Communication", 6}, {"Teamwork", 5}}, 22);
        
        // Senior Engineer -> Principal Engineer
        promotionTasks.emplace_back("Drive Cross-Team Architecture", 
            "Design and implement solutions spanning multiple teams",
            JobLevel::SENIOR_ENGINEER, 
            std::map<std::string, int>{{"Leadership", 8}, {"Problem_Solving", 8}, {"Presentation", 6}}, 28);
        
        // Principal Engineer -> Distinguished Engineer
        promotionTasks.emplace_back("Establish Technical Strategy", 
            "Define technical direction and standards for the organization",
            JobLevel::PRINCIPAL_ENGINEER, 
            std::map<std::string, int>{{"Leadership", 10}, {"Problem_Solving", 10}, {"Presentation", 8}}, 35);
        
        // Distinguished Engineer -> Fellow
        promotionTasks.emplace_back("Shape Industry Standards", 
            "Influence technical standards and practices across the industry",
            JobLevel::DISTINGUISHED_ENGINEER, 
            std::map<std::string, int>{{"Leadership", 12}, {"Problem_Solving", 12}, {"Presentation", 10}, {"Communication", 10}}, 45);
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
        if (name.empty() || name == "cancel" || name == "exit") {
            std::cout << "Character creation cancelled.\n";
            return;
        }
        characters.emplace_back(name);
        std::cout << name << " joined the meeting group!\n";
    }

    void removeCharacter() {
        clearScreen();
        if (characters.empty()) {
            std::cout << "No team members to remove!\n";
            std::cout << "Press Enter to continue...";
            std::cin.ignore();
            std::cin.get();
            return;
        }

        displayCharacters();
        std::cout << "\nSelect team member to remove (1-" << characters.size() 
                  << ", or 0 to cancel): ";
        int choice;
        std::cin >> choice;

        if (choice == 0) {
            std::cout << "Character removal cancelled.\n";
        } else if (choice >= 1 && choice <= characters.size()) {
            std::string removedName = characters[choice - 1].getName();
            characters.erase(characters.begin() + choice - 1);
            std::cout << removedName << " has left the team.\n";
        } else {
            std::cout << "Invalid selection!\n";
        }
        
        std::cout << "Press Enter to continue...";
        std::cin.ignore();
        std::cin.get();
    }

    void displayCharacters() const {
        std::cout << "\n=== Team Members (Day " << currentDay << ") ===\n";
        for (size_t i = 0; i < characters.size(); ++i) {
            std::cout << i + 1 << ". " << characters[i].getName() 
                      << " (" << characters[i].getJobLevelString()
                      << ", Level " << characters[i].getLevel() 
                      << ", Activities: " << characters[i].getActivitiesLeft() << "/3";
            if (characters[i].isEligibleForPromotion()) {
                std::cout << ", PROMOTION READY!";
            }
            std::cout << ")\n";
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

    bool attemptPromotionTask(int charIndex) {
        if (charIndex < 0 || charIndex >= characters.size()) {
            std::cout << "Invalid character selection!\n";
            return false;
        }

        Character& character = characters[charIndex];
        
        if (!character.isEligibleForPromotion()) {
            std::cout << character.getName() << " is not eligible for promotion yet!\n";
            return false;
        }
        
        if (!character.canDoActivity()) {
            std::cout << character.getName() << " has no activities left today!\n";
            return false;
        }

        // Find the promotion task for this character's current level
        PromotionTask* promotionTask = nullptr;
        for (auto& task : promotionTasks) {
            if (task.requiredLevel == character.getJobLevel()) {
                promotionTask = &task;
                break;
            }
        }

        if (!promotionTask) {
            std::cout << character.getName() << " is already at the highest level!\n";
            return false;
        }

        std::cout << "\n=== PROMOTION ATTEMPT ===\n";
        std::cout << "Task: " << promotionTask->name << "\n";
        std::cout << promotionTask->description << "\n\n";

        character.useActivity();

        // Check skill requirements
        bool meetsRequirements = true;
        std::cout << "Skill Requirements Check:\n";
        for (const auto& req : promotionTask->skillRequirements) {
            int currentSkill = character.getSkill(req.first);
            std::cout << "  " << req.first << ": " << currentSkill 
                      << "/" << req.second;
            if (currentSkill >= req.second) {
                std::cout << " ✓\n";
            } else {
                std::cout << " ✗\n";
                meetsRequirements = false;
            }
        }

        if (!meetsRequirements) {
            std::cout << "\nFAILED! Skills not sufficient for promotion.\n";
            character.gainExperience(25); // Consolation XP
            return false;
        }

        // Roll for success
        int roll = dice(rng);
        int totalBonus = 0;
        for (const auto& req : promotionTask->skillRequirements) {
            totalBonus += character.getSkill(req.first);
        }
        
        int totalScore = roll + totalBonus;
        std::cout << "\nPromotion Roll: " << roll << " + Skills(" << totalBonus 
                  << ") = " << totalScore << " vs " << promotionTask->difficulty << "\n";

        if (totalScore >= promotionTask->difficulty) {
            character.attemptPromotion();
            return true;
        } else {
            std::cout << "FAILED! Not quite ready for promotion. Keep developing skills!\n";
            character.gainExperience(50); // Good XP for trying
            return false;
        }
    }

    void attemptPromotion() {
        clearScreen();
        if (characters.empty()) {
            std::cout << "No team members available!\n";
            std::cout << "Press Enter to continue...";
            std::cin.ignore();
            std::cin.get();
            return;
        }

        // Show only characters eligible for promotion
        std::vector<int> eligibleChars;
        std::cout << "\n=== Characters Eligible for Promotion ===\n";
        for (size_t i = 0; i < characters.size(); ++i) {
            if (characters[i].isEligibleForPromotion()) {
                eligibleChars.push_back(i);
                std::cout << eligibleChars.size() << ". " << characters[i].getName() 
                          << " (" << characters[i].getJobLevelString() << ")\n";
            }
        }

        if (eligibleChars.empty()) {
            std::cout << "No characters are eligible for promotion!\n";
            std::cout << "Characters need sufficient experience and must meet skill requirements.\n";
            std::cout << "Press Enter to continue...";
            std::cin.ignore();
            std::cin.get();
            return;
        }

        std::cout << "\nSelect character for promotion (1-" << eligibleChars.size() 
                  << ", or 0 to cancel): ";
        int choice;
        std::cin >> choice;

        if (choice == 0) {
            std::cout << "Promotion cancelled.\n";
        } else if (choice >= 1 && choice <= eligibleChars.size()) {
            attemptPromotionTask(eligibleChars[choice - 1]);
        } else {
            std::cout << "Invalid selection!\n";
        }
        
        std::cout << "\nPress Enter to continue...";
        std::cin.ignore();
        std::cin.get();
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
            std::cout << "2. Remove Team Member\n";
            std::cout << "3. Do Activity\n";
            std::cout << "4. Attempt Promotion\n";
            std::cout << "5. View Team Stats\n";
            std::cout << "6. View Available Tasks\n";
            std::cout << "7. Next Day\n";
            std::cout << "8. Exit\n";
            std::cout << "Choice: ";

            int choice;
            std::cin >> choice;

            switch (choice) {
                case 1: {
                    clearScreen();
                    std::cout << "Enter team member name (or 'cancel' to cancel): ";
                    std::string name;
                    std::cin >> name;
                    addCharacter(name);
                    std::cout << "Press Enter to continue...";
                    std::cin.ignore();
                    std::cin.get();
                    break;
                }
                case 2:
                    removeCharacter();
                    break;
                case 3:
                    playRound();
                    break;
                case 4:
                    attemptPromotion();
                    break;
                case 5:
                    showStats();
                    break;
                case 6:
                    clearScreen();
                    displayTasks();
                    std::cout << "Press Enter to continue...";
                    std::cin.ignore();
                    std::cin.get();
                    break;
                case 7:
                    nextDay();
                    break;
                case 8:
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