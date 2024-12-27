

#include "csvstream.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <iterator>
#include <map>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <utility>

using namespace std;

class Classifier {
private:
    int numPosts;
    double numUniqueWords;

// For each word , the number of posts in the entire training set that contain word
    map<string, double> word_posts;

// For each label , the number of posts with that label
    map<string, double> label_posts;

// For each label C and word w, the number of posts with label C that contain w.
    map<string, map<string, double>> word_label;
    set<string> uniqueWords;

// likelihoods of each word of each label
    map<string, map<string, double>> log_likelihoods;

// The likelihood of each label
    map<string, double> label_likelihood = label_posts;

    map<string, double> temp_label_likelihood;


public:
    // EFFECTS: Returns a set containing the unique "words" in the original
    //          string, delimited by whitespace.
    set<string> unique_words(const string &str) {
    // Fancy modern C++ and STL way to do it
    istringstream source{str};
    return {istream_iterator<string>{source},
            istream_iterator<string>{}};
    }




    void training_classifier(csvstream &train_file, bool debug) {
        numPosts = 0;      
        map <string, string> line;
        if (debug) cout << "training data:" << endl;
        while (train_file >> line) {
            numPosts++;
            label_posts[line["tag"]] += 1;
            uniqueWords = unique_words(line["content"]);
            set<string>::iterator i;
            for (i = uniqueWords.begin(); i != uniqueWords.end(); ++i) {
                word_posts[*i] += 1;
                word_label[line["tag"]][*i] += 1;
                numUniqueWords++;
            }
            if (debug == true) {
                string label;
                set<string> content;
                cout << "  label = " << line["tag"] 
                        << ", content = " << line["content"] << endl;
            }
        }
    }

    void train(csvstream &train_file, bool debug) {
        map <string, string> line;
        training_classifier(train_file, debug);
        cout << "trained on " << numPosts << " examples" << endl;
        if (debug) {
            numUniqueWords = word_posts.size();
            cout << "vocabulary size = " << numUniqueWords << endl << endl;
        }

        if (debug) {
            cout << "classes:" << endl;
        }
        double log_prior;
        for (auto const &i : label_posts) {
            double value = (i.second/((double)numPosts));
            log_prior = log(value);
            if (debug) {
                cout << "  " << i.first << ", " << i.second 
                    << " examples, log-prior = " << log_prior << endl;
            }
            label_likelihood[i.first] = log_prior;
        }
        if (debug) {
            cout << "classifier parameters:" << endl;
        }
        for(auto const &i : word_label) {
            for(auto const &j : i.second) {
                if (debug) {
                    cout << "  " << i.first << ":";
                    cout << j.first << ", count = " << j.second 
                        << ", log-likelihood = ";
                }
                double log_likely;
                if (word_posts[j.first] == 0) {
                    log_likely = log(1/((double)numPosts));
                    if (debug) cout << log_likely << endl;
                }
                else if (j.second == 0 && word_posts[j.first] > 0) {
                    log_likely = log((word_posts[j.first])/((double)numPosts));
                    if (debug) cout << log_likely << endl;
                }
                else {
                    log_likely = log((j.second)/(label_posts[i.first]));
                    if (debug) cout << log_likely << endl;
                }
                log_likelihoods[i.first][j.first] = log_likely;
            }
        }
        cout << endl;
    }





    double log_prob_zero(string word) {
        double value;
        bool exist = false;
        for (auto const &i : word_posts) {
            if (word == i.first) {
                exist = true;
            }
        }
        if (exist) {
            value = log((word_posts[word]) / ((double)numPosts));
        }
        else {
            value = log(1 / ((double)numPosts));
        }
        return value;
    }






    void test(csvstream &test_file) {
        int post_count = 0;
        int post_correct = 0;
        cout << "test data:" << endl;
        map <string, string> line;
        pair<string, double> predictor;
        set<string> words;


        while (test_file >> line) {
            temp_label_likelihood = label_likelihood;
            double value = 0;
            map<string, double>::iterator check;
            words = unique_words(line["content"]);
            
            for (auto const &i : label_likelihood) {
                double log = 0;
                for (auto const &j : words) {
                    check = (log_likelihoods[i.first].find(j));
                    if (check == log_likelihoods[i.first].end()) {
                        value = log_prob_zero(j);
                    }
                    else {
                        value = log_likelihoods[i.first][j];
                    }
                    log += value;
                }
                label_likelihood[i.first] += log;
            }

            pair<string, double> entryWithMaxValue("null", -99999999);
            map<string, double>::iterator it;
            for (it = label_likelihood.begin(); it != label_likelihood.end(); ++it) {
                if ((it->second) > (entryWithMaxValue.second)) {
                    entryWithMaxValue = {it->first, it->second};
                }
            }
            predictor = {entryWithMaxValue.first, entryWithMaxValue.second};

            cout << "  correct = " << line["tag"] << ", predicted = " << 
                    predictor.first << ", log-probability score = " 
                        << predictor.second << endl;
            cout << "  content = " << line["content"] << endl;
            cout << endl;

            if (predictor.first == line["tag"]) {
                post_correct++;
                post_count++;
            }
            else {
                post_count++;
            }
            label_likelihood = temp_label_likelihood;
        }
        cout << "performance: " << post_correct << " / "
        << post_count << " posts predicted correctly" << endl;
    }
};



int main(int argc, char *argv[]) {
    cout.precision(3);
    bool debug = false;
    if (argc != 3 && argc != 4) {
        cout << "Usage: main.exe TRAIN_FILE TEST_FILE [--debug]" << endl;
        return -1;
    }
    if (argc == 4 && !(strcmp(argv[3], "--debug") == 0)) {
        cout << "Usage: main.exe TRAIN_FILE TEST_FILE [--debug]" << endl;
        return -1;
    }
    else if (argc == 4 && (strcmp(argv[3], "--debug") == 0)) {
        debug = true;
    }
    csvstream train_file(argv[1]);
    csvstream test_file(argv[2]);
    if (!train_file) {
        cout << "Error opening file: " << argv[1] << endl;
        return 1;
    }
    if (!test_file) {
        cout << "Error opening file: " << argv[2] << endl;
        return 1;
    }
    Classifier c;
    c.train(train_file, debug);
    c.test(test_file);
}