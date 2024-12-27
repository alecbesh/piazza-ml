

#include <cassert>
#include <cmath>
#include <algorithm>
#include <set>
#include <string>
#include <iostream>
#include "csvstream.h"
#include <map>
#include <cstring>

using namespace std;

int global_post_count;
int global_word_count;
map<string, int> post_words, num_label;
map<string, map<string, int>> num_word_label;
bool debug;

set<string> unique_words(const string & str);
double get_freq(string word_in, string label_in);
void train_ml(csvstream *train_in);
void analyze(csvstream *test_in);
void output_debug();


set<string> unique_words(const string &str) {
  string word = "";
  size_t len = str.length();
  set<string> words;
  for (int i = 0; i < len; i ++){
    char curr = str.at(i);
    if (i == (len - 1)) {
      word += curr;
      words.insert(word);
      break;
    }
    else if(curr != ' ') {
      word += curr;
    }
    else{
      words.insert(word);
      word = "";
    }
  }
//sets automatically reject duplicate entries.
  return words;
}

double get_freq(string word_in, string label_in) {
  double likelihood_log = 0;
  if (post_words.find(word_in) == post_words.end()) {
      double curr = pow(global_post_count, -1);
    likelihood_log = log(curr);
  }
  else if ((post_words.find(word_in) != post_words.end())
           && (num_word_label[label_in].find(word_in) ==
               num_word_label[label_in].end())) {
    double curr = pow(global_post_count, -1);
    likelihood_log = log(post_words[word_in] * curr);
  }
  else {
    likelihood_log = log(num_word_label[label_in][word_in] *
      pow(num_label[label_in], -1));
  }
  return likelihood_log;
}

void train_ml(csvstream *train_in) {
    if (debug) {
        cout << "training data:" << endl;
    }
    map <string, string> current_line;
    //while still data in file
    while ((*train_in) >> current_line) {
        ++ global_post_count;
        num_label[current_line["tag"]] += 1;
        set<string> data = unique_words(current_line["content"]);
        set<string>::iterator i;
        for (i = data.begin(); i != data.end(); ++i) {
            post_words[*i] += 1;
            num_word_label[current_line["tag"]][* i] += 1;
        }
        if (debug) {
            cout << "  label = " << current_line["tag"] << ", content = "
            << current_line["content"] << endl;
        }
    }
    global_word_count = (int)post_words.size();
    cout << "trained on " << global_post_count << " examples" << endl;
  if (debug) {
      output_debug();
  }
  cout << endl << "test data:" << endl;
}

void analyze(csvstream *test_in) {
    int posts_confirmed = 0;
    int post_count = 0;
    map <string, string> current_line;
    while (*(test_in) >> current_line) {
        //set to lowest so cant be lower
        pair<string, double> prediction("null", -__DBL_MAX__);
        set<string> data = unique_words(current_line["content"]);
        for (pair<string, int> label : num_label) {
          string temp_label = label.first;
          double gb_f = pow(global_post_count, -1);
          double label_correct_freq = log(num_label[temp_label] * gb_f);
          for (auto word : data) {
              label_correct_freq += get_freq(word, temp_label);
          }
          if (label_correct_freq > prediction.second) {
            prediction = make_pair(temp_label, label_correct_freq);
          }
        }
        string pred1 = prediction.first;
        double pred2 = prediction.second;
        cout << "  correct = " << current_line["tag"] << ", predicted = "
        << pred1 << ", log-probability score = " << pred2 << endl
        << "  content = " << current_line["content"] << endl << endl;
        if (pred1 == current_line["tag"]) {
            ++ posts_confirmed;
            ++ post_count;
        }
        else {
            ++ post_count;
        }
    }
  cout << "performance: " << posts_confirmed << " / "
        << post_count << " posts predicted correctly" << endl;
}

void output_debug() {
    cout << "vocabulary size = " << global_word_count << endl << endl
    << "classes:" << endl;
    map<string, int>::iterator i;
    for (i = num_label.begin(); i != num_label.end(); ++i) {
    string label_in = i->first;
    double curr = pow(global_post_count, -1);
    double prev_log = log(num_label[label_in] * curr);
    cout << "  " << label_in << ", " << i->second << " examples," <<
      " log-prior = " << prev_log << endl;
    }
    cout << "classifier parameters:" << endl;
    map<string, map<string, int>>::iterator it;
  for (it = num_word_label.begin(); it != num_word_label.end(); ++it) {
    string label_in = it -> first;
    map <string, int>::iterator i2;
    for (i2 = num_word_label[label_in].begin();
         i2 != num_word_label[label_in].end(); ++i2) {
      string word_in = i2 -> first;
      double likelihood_log = get_freq(word_in, label_in);
      int cur_count = num_word_label[label_in][word_in];
      cout << "  " << label_in << ":" << word_in << ", count = " << cur_count
        << ", log-likelihood = " << likelihood_log << endl;
    }
  }
}

int main(int argc, char * argv[]) {
    cout.precision(3);
    debug = (argc == 4);
    global_word_count = 0;
    global_post_count = 0;
    bool acceptable_form = true;
    //invalid # params
    if (argc < 3 || argc > 4) {
      cout << "Usage: main.exe TRAIN_FILE TEST_FILE [--debug]" << endl;
      acceptable_form = false;
    }
    //if 4, debug
    if (argc == 4) {
        //-- not included
      bool correct_form = strcmp(argv[3], "debug");
      if (!correct_form) {
        cout << "Usage: main.exe TRAIN_FILE TEST_FILE [--debug]" << endl;
        acceptable_form = false;
      }
    }
    if (!acceptable_form) {
        return 1;
    }
    csvstream train_in(argv[1]);
    csvstream test_in(argv[2]);
    if (!train_in) {
        cout << "Error opening file: " << argv[1] << endl;
        return 1;
    }
    if (!test_in) {
        cout << "Error opening file: " << argv[2] << endl;
        return 1;
    }
    train_ml(&train_in);
    analyze(&test_in);
}