#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <cmath>     // For ceil() and log2()
#include <climits>   // For SIZE_MAX

using namespace std;
using namespace chrono;

// Structure to hold word and its definition
struct DictionaryEntry {
    string word;
    string definition;
    string partOfSpeech;
    
    DictionaryEntry() = default;
    
    DictionaryEntry(string w, string d, string pos = "") 
        : word(w), definition(d), partOfSpeech(pos) {}
    
    // For sorting algorithms
    bool operator<(const DictionaryEntry& other) const {
        return word < other.word;
    }
    
    bool operator>(const DictionaryEntry& other) const {
        return word > other.word;
    }
};

class Dictionary {
private:
    vector<DictionaryEntry> entries;
    string fileName;
    
    // Convert string to lowercase for case-insensitive comparison
    static string toLower(string str) {
        transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }
    
    // BUBBLE SORT - O(n²)
    void bubbleSort() {
        int n = entries.size();
        bool swapped;
        
        cout << "Applying Bubble Sort..." << endl;
        auto start = high_resolution_clock::now();
        
        for (int i = 0; i < n - 1; i++) {
            swapped = false;
            for (int j = 0; j < n - i - 1; j++) {
                // Case-insensitive comparison
                if (toLower(entries[j].word) > toLower(entries[j + 1].word)) {
                    swap(entries[j], entries[j + 1]);
                    swapped = true;
                }
            }
            // If no swapping occurred, array is already sorted
            if (!swapped) {
                break;
            }
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Bubble Sort completed in " << duration.count() << " microseconds!" << endl;
    }
    
    // QUICK SORT - O(n log n) average case
    void quickSort(int low, int high) {
        if (low < high) {
            int pi = partition(low, high);
            quickSort(low, pi - 1);
            quickSort(pi + 1, high);
        }
    }
    
    int partition(int low, int high) {
        // Choose middle element as pivot for better performance
        int mid = low + (high - low) / 2;
        string pivot = toLower(entries[mid].word);
        
        // Move pivot to end
        swap(entries[mid], entries[high]);
        
        int i = low - 1;
        
        for (int j = low; j < high; j++) {
            if (toLower(entries[j].word) <= pivot) {
                i++;
                swap(entries[i], entries[j]);
            }
        }
        
        swap(entries[i + 1], entries[high]);
        return i + 1;
    }
    
    // BINARY SEARCH - O(log n)
    int binarySearch(const string& word) {
        int left = 0;
        int right = entries.size() - 1;
        string lowerWord = toLower(word);
        
        while (left <= right) {
            int mid = left + (right - left) / 2;
            string midWord = toLower(entries[mid].word);
            
            if (midWord == lowerWord) {
                return mid;
            }
            else if (midWord < lowerWord) {
                left = mid + 1;
            }
            else {
                right = mid - 1;
            }
        }
        return -1;
    }
    
    // Check if dictionary is sorted
    bool isSorted() {
        for (size_t i = 1; i < entries.size(); i++) {
            if (toLower(entries[i].word) < toLower(entries[i-1].word)) {
                return false;
            }
        }
        return true;
    }

public:
    Dictionary() {
        fileName = "dictionary.txt";
        loadFromFile();
    }
    
    // Load dictionary from file
    void loadFromFile() {
        ifstream file(fileName);
        if (!file.is_open()) {
            cout << "No existing dictionary file found. Starting fresh!" << endl;
            return;
        }
        
        entries.clear();
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            
            size_t pos1 = line.find('|');
            size_t pos2 = line.find('|', pos1 + 1);
            
            if (pos1 != string::npos) {
                string word = line.substr(0, pos1);
                string definition = line.substr(pos1 + 1, pos2 - pos1 - 1);
                string pos = (pos2 != string::npos) ? line.substr(pos2 + 1) : "";
                entries.push_back(DictionaryEntry(word, definition, pos));
            }
        }
        file.close();
        
        // Sort entries after loading using Quick Sort
        if (!entries.empty()) {
            cout << "Sorting loaded entries with Quick Sort..." << endl;
            quickSort(0, entries.size() - 1);
        }
        
        cout << "Loaded " << entries.size() << " entries from dictionary!" << endl;
    }
    
    // Save dictionary to file
    void saveToFile() {
        ofstream file(fileName);
        if (!file.is_open()) {
            cout << "Error: Cannot save dictionary to file!" << endl;
            return;
        }
        
        for (const auto& entry : entries) {
            file << entry.word << "|" << entry.definition << "|" << entry.partOfSpeech << endl;
        }
        file.close();
        cout << "Dictionary saved successfully!" << endl;
    }
    
    // Add new word with definition
    void addWord() {
        string word, definition, pos;
        cout << "Enter word: ";
        cin.ignore();
        getline(cin, word);
        
        // Check if dictionary is sorted before search
        if (!isSorted()) {
            cout << "Dictionary not sorted! Sorting with Quick Sort..." << endl;
            quickSort(0, entries.size() - 1);
        }
        
        // Check if word already exists using Binary Search
        int index = binarySearch(word);
        if (index != -1) {
            cout << "Word already exists! Use update option to modify." << endl;
            return;
        }
        
        cout << "Enter definition: ";
        getline(cin, definition);
        cout << "Enter part of speech (noun/verb/adjective/etc.): ";
        getline(cin, pos);
        
        entries.push_back(DictionaryEntry(word, definition, pos));
        
        // Sort after addition using Quick Sort
        cout << "Re-sorting dictionary with Quick Sort..." << endl;
        quickSort(0, entries.size() - 1);
        saveToFile();
        
        cout << "Word added successfully!" << endl;
    }
    
    // Search for a word using Binary Search
    void searchWord() {
        if (entries.empty()) {
            cout << "Dictionary is empty!" << endl;
            return;
        }
        
        // Ensure dictionary is sorted for binary search
        if (!isSorted()) {
            cout << "Dictionary not sorted! Sorting with Quick Sort before search..." << endl;
            quickSort(0, entries.size() - 1);
        }
        
        string word;
        cout << "Enter word to search: ";
        cin.ignore();
        getline(cin, word);
        
        cout << "\n=== Binary Search Results ===" << endl;
        auto start = high_resolution_clock::now();
        int index = binarySearch(word);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        
        if (index != -1) {
            cout << "✅ Word Found!" << endl;
            cout << "Word: " << entries[index].word << endl;
            cout << "Definition: " << entries[index].definition << endl;
            if (!entries[index].partOfSpeech.empty()) {
                cout << "Part of Speech: " << entries[index].partOfSpeech << endl;
            }
            cout << "Search Time: " << duration.count() << " microseconds" << endl;
        } else {
            cout << "❌ Word not found in dictionary!" << endl;
            cout << "Search Time: " << duration.count() << " microseconds" << endl;
            
            // Suggest similar words (simple prefix matching)
            suggestSimilarWords(word);
        }
    }
    
    // Suggest similar words using prefix matching
    void suggestSimilarWords(const string& word) {
        cout << "\n--- Suggestions (words starting with similar prefix) ---" << endl;
        string lowerWord = toLower(word);
        int suggestions = 0;
        
        for (const auto& entry : entries) {
            string lowerEntry = toLower(entry.word);
            if (lowerEntry.find(lowerWord) == 0) { // Prefix match
                cout << "• " << entry.word << endl;
                suggestions++;
                if (suggestions >= 5) break;
            }
        }
        
        if (suggestions == 0) {
            // Show first few words if no prefix matches
            cout << "No words with similar prefix found. Here are some words:" << endl;
            for (size_t i = 0; i < min((size_t)5, entries.size()); i++) {
                cout << "• " << entries[i].word << endl;
            }
        }
    }
    
    // Update existing word
    void updateWord() {
        if (entries.empty()) {
            cout << "Dictionary is empty!" << endl;
            return;
        }
        
        // Ensure dictionary is sorted
        if (!isSorted()) {
            cout << "Dictionary not sorted! Sorting with Quick Sort..." << endl;
            quickSort(0, entries.size() - 1);
        }
        
        string word;
        cout << "Enter word to update: ";
        cin.ignore();
        getline(cin, word);
        
        int index = binarySearch(word);
        if (index == -1) {
            cout << "Word not found!" << endl;
            return;
        }
        
        cout << "Current definition: " << entries[index].definition << endl;
        cout << "Current part of speech: " << entries[index].partOfSpeech << endl;
        cout << "Enter new definition: ";
        string newDefinition;
        getline(cin, newDefinition);
        cout << "Enter new part of speech: ";
        string newPos;
        getline(cin, newPos);
        
        entries[index].definition = newDefinition;
        entries[index].partOfSpeech = newPos;
        saveToFile();
        
        cout << "Word updated successfully!" << endl;
    }
    
    // Delete a word
    void deleteWord() {
        if (entries.empty()) {
            cout << "Dictionary is empty!" << endl;
            return;
        }
        
        // Ensure dictionary is sorted
        if (!isSorted()) {
            cout << "Dictionary not sorted! Sorting with Quick Sort..." << endl;
            quickSort(0, entries.size() - 1);
        }
        
        string word;
        cout << "Enter word to delete: ";
        cin.ignore();
        getline(cin, word);
        
        int index = binarySearch(word);
        if (index == -1) {
            cout << "Word not found!" << endl;
            return;
        }
        
        cout << "Deleting: " << entries[index].word << endl;
        entries.erase(entries.begin() + index);
        saveToFile();
        
        cout << "Word deleted successfully!" << endl;
    }
    
    // Display all words A-Z
    void displayAllWords() {
        if (entries.empty()) {
            cout << "Dictionary is empty!" << endl;
            return;
        }
        
        // Ensure dictionary is sorted for proper display
        if (!isSorted()) {
            cout << "Dictionary not sorted! Sorting with Quick Sort for display..." << endl;
            quickSort(0, entries.size() - 1);
        }
        
        cout << "\n=== Dictionary Contents (A-Z) ===" << endl;
        cout << left << setw(25) << "Word" 
             << setw(40) << "Definition"
             << setw(15) << "Part of Speech" << endl;
        cout << string(80, '-') << endl;
        
        char currentLetter = '\0';
        for (const auto& entry : entries) {
            if (!entry.word.empty()) {
                char firstLetter = toupper(entry.word[0]);
                if (firstLetter != currentLetter) {
                    currentLetter = firstLetter;
                    cout << "\n--- " << currentLetter << " ---" << endl;
                }
                cout << left << setw(25) << entry.word.substr(0, 24)
                     << setw(40) << entry.definition.substr(0, 39)
                     << setw(15) << entry.partOfSpeech.substr(0, 14) << endl;
            }
        }
        cout << endl;
    }
    
    // Sort dictionary using user-selected algorithm
    void sortDictionary() {
        if (entries.empty()) {
            cout << "Dictionary is empty! Nothing to sort." << endl;
            return;
        }
        
        cout << "\n=== Sorting Algorithms ===" << endl;
        cout << "1. Bubble Sort (O(n²) - Simple but slow for large data)" << endl;
        cout << "2. Quick Sort (O(n log n) - Fast and efficient)" << endl;
        cout << "Enter your choice: ";
        
        int choice;
        cin >> choice;
        
        switch(choice) {
            case 1:
                bubbleSort();
                break;
            case 2:
                cout << "Applying Quick Sort..." << endl;
                auto start = high_resolution_clock::now();
                quickSort(0, entries.size() - 1);
                auto end = high_resolution_clock::now();
                auto duration = duration_cast<microseconds>(end - start);
                cout << "Quick Sort completed in " << duration.count() << " microseconds!" << endl;
                break;
        }
        
        // Verify sorting
        if (isSorted()) {
            cout << "✅ Dictionary is now sorted correctly!" << endl;
            saveToFile();
        } else {
            cout << "❌ Error: Dictionary is not sorted properly!" << endl;
        }
    }
    
    // Get statistics
    void getStatistics() {
        if (entries.empty()) {
            cout << "Dictionary is empty!" << endl;
            return;
        }
        
        // Ensure dictionary is sorted for statistics
        if (!isSorted()) {
            cout << "Dictionary not sorted! Sorting with Quick Sort for statistics..." << endl;
            quickSort(0, entries.size() - 1);
        }
        
        cout << "\n=== Dictionary Statistics ===" << endl;
        cout << "Total words: " << entries.size() << endl;
        cout << "First word: " << entries[0].word << endl;
        cout << "Last word: " << entries.back().word << endl;
        
        // Count words by first letter
        cout << "\nWord count by letter:" << endl;
        vector<int> letterCount(26, 0);
        for (const auto& entry : entries) {
            if (!entry.word.empty()) {
                char first = toupper(entry.word[0]);
                if (first >= 'A' && first <= 'Z') {
                    letterCount[first - 'A']++;
                }
            }
        }
        
        for (int i = 0; i < 26; i++) {
            if (letterCount[i] > 0) {
                cout << "  " << char('A' + i) << ": " << letterCount[i] << " words" << endl;
            }
        }
        
        // Find longest and shortest words
        string longestWord, shortestWord;
        size_t maxLen = 0, minLen = SIZE_MAX;
        
        for (const auto& entry : entries) {
            if (entry.word.length() > maxLen) {
                maxLen = entry.word.length();
                longestWord = entry.word;
            }
            if (entry.word.length() < minLen) {
                minLen = entry.word.length();
                shortestWord = entry.word;
            }
        }
        
        cout << "\nLongest word: " << longestWord << " (" << maxLen << " characters)" << endl;
        cout << "Shortest word: " << shortestWord << " (" << minLen << " characters)" << endl;
    }
    
    // Demonstrate binary search efficiency
    void demonstrateBinarySearch() {
        if (entries.empty()) {
            cout << "Dictionary is empty! Add some words first." << endl;
            return;
        }
        
        // Ensure dictionary is sorted
        if (!isSorted()) {
            cout << "Sorting dictionary with Quick Sort for demonstration..." << endl;
            quickSort(0, entries.size() - 1);
        }
        
        cout << "\n=== Binary Search Demonstration ===" << endl;
        cout << "Dictionary size: " << entries.size() << " words" << endl;
        
        // Calculate and display theoretical maximum comparisons
        int maxComparisons = 0;
        int temp = entries.size();
        while (temp > 0) {
            maxComparisons++;
            temp /= 2;
        }
        
        cout << "Maximum comparisons needed: log₂(" << entries.size() 
             << ") ≈ " << maxComparisons << " comparisons" << endl;
        
        string word;
        cout << "\nEnter a word to search (or 'quit' to exit): ";
        cin.ignore();
        getline(cin, word);
        
        if (word != "quit") {
            auto start = high_resolution_clock::now();
            int comparisons = 0;
            
            // Binary search with comparison counter
            int left = 0, right = entries.size() - 1;
            string lowerWord = toLower(word);
            int index = -1;
            
            while (left <= right) {
                comparisons++;
                int mid = left + (right - left) / 2;
                string midWord = toLower(entries[mid].word);
                
                if (midWord == lowerWord) {
                    index = mid;
                    break;
                }
                else if (midWord < lowerWord) {
                    left = mid + 1;
                }
                else {
                    right = mid - 1;
                }
            }
            
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(end - start);
            
            if (index != -1) {
                cout << "\n✅ Word found!" << endl;
                cout << "Comparisons made: " << comparisons << endl;
                cout << "Time taken: " << duration.count() << " microseconds" << endl;
                cout << "\nWord Details:" << endl;
                cout << "  Word: " << entries[index].word << endl;
                cout << "  Definition: " << entries[index].definition << endl;
                cout << "  Part of Speech: " << entries[index].partOfSpeech << endl;
            } else {
                cout << "\n❌ Word not found!" << endl;
                cout << "Comparisons made: " << comparisons << endl;
                cout << "Time taken: " << duration.count() << " microseconds" << endl;
            }
        }
    }
};

int main() {
    Dictionary dict;
    int choice;
    
    cout << "================================================" << endl;
    cout << "      DICTIONARY APPLICATION                    " << endl;
    cout << "   Algorithms: Bubble Sort | Quick Sort | Binary Search" << endl;
    cout << "================================================" << endl;
    
    do {
        cout << "\n========== MAIN MENU ==========" << endl;
        cout << "1. Add New Word" << endl;
        cout << "2. Search Word (Binary Search)" << endl;
        cout << "3. Update Word Definition" << endl;
        cout << "4. Delete Word" << endl;
        cout << "5. Display All Words (A-Z)" << endl;
        cout << "6. Sort Dictionary (Bubble Sort / Quick Sort)" << endl;
        cout << "7. Dictionary Statistics" << endl;
        cout << "8. Demonstrate Binary Search Efficiency" << endl;
        cout << "9. Save Dictionary" << endl;
        cout << "0. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        
        switch(choice) {
            case 1:
                dict.addWord();
                break;
            case 2:
                dict.searchWord();
                break;
            case 3:
                dict.updateWord();
                break;
            case 4:
                dict.deleteWord();
                break;
            case 5:
                dict.displayAllWords();
                break;
            case 6:
                dict.sortDictionary();
                break;
            case 7:
                dict.getStatistics();
                break;
            case 8:
                dict.demonstrateBinarySearch();
                break;
            case 9:
                dict.saveToFile();
                break;
            case 0:
                dict.saveToFile();
                cout << "\nThank you for using Dictionary Application!" << endl;
                cout << "Implemented Sorting: Bubble Sort & Quick Sort" << endl;
                cout << "Implemented Searching: Binary Search" << endl;
                break;
            default:
                cout << "Invalid choice! Please try again." << endl;
        }
        
    } while (choice != 0);
    
    return 0;
}