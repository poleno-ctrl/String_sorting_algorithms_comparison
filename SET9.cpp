#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <chrono>
#include <fstream>

using namespace std;
using namespace std::chrono;

long long char_comparisons = 0;

int cmp_strings(const string& a, const string& b) {
    size_t len = min(a.length(), b.length());
    for (size_t i = 0; i < len; ++i) {
        char_comparisons++;
        if (a[i] != b[i]) {
            return a[i] < b[i] ? -1 : 1;
        }
    }
    char_comparisons++;
    if (a.length() == b.length()) return 0;
    return a.length() < b.length() ? -1 : 1;
}

class StringGenerator {
private:
    const string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#%:;^&*()-";
    mt19937 rng;

public:
    StringGenerator() : rng(42) {}

    string generateRandomString(int min_len = 10, int max_len = 200) {
        uniform_int_distribution<int> len_dist(min_len, max_len);
        uniform_int_distribution<int> char_dist(0, alphabet.size() - 1);
        int len = len_dist(rng);
        string s(len, ' ');
        for (int i = 0; i < len; i++) {
            s[i] = alphabet[char_dist(rng)];
        }
        return s;
    }

    vector<string> generateRandom(int size) {
        vector<string> res(size);
        string prefix = generateRandomString(5, 15);

        for (int i = 0; i < size; ++i) {
            res[i] = prefix + generateRandomString(5, 185);
        }
        return res;
    }

    vector<string> generateReverseSorted(int size) {
        vector<string> res = generateRandom(size);
        sort(res.begin(), res.end(), greater<string>());
        return res;
    }

    vector<string> generateNearlySorted(int size) {
        vector<string> res = generateRandom(size);
        sort(res.begin(), res.end());
        uniform_int_distribution<int> idx_dist(0, size - 1);
        int swaps = size * 0.05;
        for (int i = 0; i < swaps; ++i) {
            swap(res[idx_dist(rng)], res[idx_dist(rng)]);
        }
        return res;
    }
};

void merge(vector<string>& arr, int left, int mid, int right) {
    vector<string> temp(right - left + 1);
    int i = left, j = mid + 1, k = 0;
    while (i <= mid && j <= right) {
        if (cmp_strings(arr[i], arr[j]) <= 0) {
            temp[k++] = arr[i++];
        }
        else {
            temp[k++] = arr[j++];
        }
    }
    while (i <= mid) {
        temp[k++] = arr[i++];
    }
    while (j <= right) {
        temp[k++] = arr[j++];
    }
    for (int p = 0; p < k; ++p) {
        arr[left + p] = temp[p];
    }
}

void std_mergesort(vector<string>& arr, int left, int right) {
    if (left >= right) return;
    int mid = (left + right) / 2;
    std_mergesort(arr, left, mid);
    std_mergesort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

void std_quicksort(vector<string>& arr, int left, int right) {
    if (left >= right) return;
    string med = arr[(left + right) / 2];
    int i = left, j = right;
    while (i <= j) {
        while (cmp_strings(arr[i], med) < 0) {
            i++;
        }
        while (cmp_strings(arr[j], med) > 0) {
            j--;
        }
        if (i <= j) {
            swap(arr[i], arr[j]);
            i++; 
            j--;
        }
    }
    std_quicksort(arr, left, j);
    std_quicksort(arr, i, right);
}

void string_merge_lcp(vector<string>& arr, vector<int>& LCP, int left, int mid, int right) {
    vector<string> temp_arr(right - left + 1);
    vector<int> temp_lcp(right - left + 1);

    int i = left, j = mid + 1, k = 0;

    int curr_lcp = 0;
    int max_len = min(arr[i].length(), arr[j].length());
    while (curr_lcp < max_len) {
        char_comparisons++;
        if (arr[i][curr_lcp] == arr[j][curr_lcp]) curr_lcp++;
        else break;
    }
    if (curr_lcp < max_len) char_comparisons++;

    while (i <= mid && j <= right) {
        bool i_is_smaller = false;

        if (curr_lcp == arr[i].length() && curr_lcp == arr[j].length()) {
            i_is_smaller = true;
        }
        else if (curr_lcp == arr[i].length()) {
            i_is_smaller = true; 
        }
        else if (curr_lcp == arr[j].length()) {
            i_is_smaller = false;
        }
        else {
            i_is_smaller = (arr[i][curr_lcp] <= arr[j][curr_lcp]);
        }

        if (i_is_smaller) {
            temp_arr[k] = arr[i];
            if (i == mid) {
                temp_lcp[k] = curr_lcp;
                k++; i++;
            }
            else {
                if (LCP[i] > curr_lcp) {
                    temp_lcp[k] = LCP[i];
                    k++; i++;
                }
                else if (LCP[i] < curr_lcp) {
                    temp_lcp[k] = curr_lcp;
                    curr_lcp = LCP[i];
                    k++; i++;
                }
                else {
                    temp_lcp[k] = curr_lcp;
                    int new_lcp = curr_lcp;
                    int mlen = min(arr[i + 1].length(), arr[j].length());
                    while (new_lcp < mlen) {
                        char_comparisons++;
                        if (arr[i + 1][new_lcp] == arr[j][new_lcp]) new_lcp++;
                        else break;
                    }
                    if (new_lcp < mlen) char_comparisons++;
                    curr_lcp = new_lcp;
                    k++; i++;
                }
            }
        }
        else {
            temp_arr[k] = arr[j];
            if (j == right) {
                temp_lcp[k] = curr_lcp;
                k++; j++;
            }
            else {
                if (LCP[j] > curr_lcp) {
                    temp_lcp[k] = LCP[j];
                    k++; j++;
                }
                else if (LCP[j] < curr_lcp) {
                    temp_lcp[k] = curr_lcp;
                    curr_lcp = LCP[j];
                    k++; j++;
                }
                else {
                    temp_lcp[k] = curr_lcp;
                    int new_lcp = curr_lcp;
                    int mlen = min(arr[i].length(), arr[j + 1].length());
                    while (new_lcp < mlen) {
                        char_comparisons++;
                        if (arr[i][new_lcp] == arr[j + 1][new_lcp]) new_lcp++;
                        else break;
                    }
                    if (new_lcp < mlen) char_comparisons++;
                    curr_lcp = new_lcp;
                    k++; j++;
                }
            }
        }
    }

    while (i <= mid) {
        temp_arr[k] = arr[i];
        temp_lcp[k] = (i < mid) ? LCP[i] : 0;
        k++; i++;
    }
    while (j <= right) {
        temp_arr[k] = arr[j];
        temp_lcp[k] = (j < right) ? LCP[j] : 0;
        k++; j++;
    }

    for (int p = 0; p < temp_arr.size(); ++p) {
        arr[left + p] = temp_arr[p];
        if (left + p < right) {
            LCP[left + p] = temp_lcp[p];
        }
    }
}

void string_mergesort_lcp(vector<string>& arr, vector<int>& LCP, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    string_mergesort_lcp(arr, LCP, left, mid);
    string_mergesort_lcp(arr, LCP, mid + 1, right);
    string_merge_lcp(arr, LCP, left, mid, right);
}

void string_mergesort(vector<string>& arr) {
    if (arr.empty()) return;
    vector<int> LCP(arr.size(), 0);
    string_mergesort_lcp(arr, LCP, 0, arr.size() - 1);
}

inline int charAt(const string& s, int d) {
    if (d < s.length()) return (unsigned char)s[d];
    return -1;
}

void string_quicksort(vector<string>& arr, int left, int right, int d) {
    if (left >= right) return;
    int pivot_idx = left + (right - left) / 2;
    int v_val = charAt(arr[pivot_idx], d);

    swap(arr[left], arr[pivot_idx]);
    int lt = left, gt = right;
    int i = left + 1;

    while (i <= gt) {
        int t = charAt(arr[i], d);

        char_comparisons++;
        if (t < v_val) {
            swap(arr[lt++], arr[i++]);
        }
        else if (t > v_val) {
            char_comparisons++;
            swap(arr[i], arr[gt--]);
        }
        else {
            char_comparisons++;
            i++;
        }
    }

    string_quicksort(arr, left, lt - 1, d);
    if (v_val >= 0) string_quicksort(arr, lt, gt, d + 1);
    string_quicksort(arr, gt + 1, right, d);
}

const int ALPHABET_SIZE = 256;
void msd_radix(vector<string>& arr, int left, int right, int d, vector<string>& aux) {
    if (right <= left) return;
    vector<int> count(ALPHABET_SIZE + 2, 0);
    for (int i = left; i <= right; ++i) {
        count[charAt(arr[i], d) + 2]++;
    }
    for (int r = 0; r < ALPHABET_SIZE + 1; ++r) {
        count[r + 1] += count[r];
    }
    for (int i = left; i <= right; ++i) {
        aux[count[charAt(arr[i], d) + 1]++] = arr[i];
    }
    for (int i = left; i <= right; ++i) {
        arr[i] = aux[i - left];
    }

    for (int r = 0; r < ALPHABET_SIZE; ++r) {
        msd_radix(arr, left + count[r], left + count[r + 1] - 1, d + 1, aux);
    }
}

const int THRESHOLD = 74;
void msd_radix_fallback(vector<string>& arr, int left, int right, int d, vector<string>& aux) {
    if (right <= left) return;
    if (right - left + 1 <= THRESHOLD) {
        string_quicksort(arr, left, right, d);
        return;
    }
    vector<int> count(ALPHABET_SIZE + 2, 0);

    for (int i = left; i <= right; ++i) {
        count[charAt(arr[i], d) + 2]++;
    }
    for (int r = 0; r < ALPHABET_SIZE + 1; ++r) {
        count[r + 1] += count[r];
    }
    for (int i = left; i <= right; ++i) {
        aux[count[charAt(arr[i], d) + 1]++] = arr[i];
    }
    for (int i = left; i <= right; ++i) {
        arr[i] = aux[i - left];
    }

    for (int r = 0; r < ALPHABET_SIZE; ++r) {
        msd_radix_fallback(arr, left + count[r], left + count[r + 1] - 1, d + 1, aux);
    }
}

class StringSortTester {
public:
    void runExperiments() {
        StringGenerator gen;
        vector<int> sizes;
        for (int i = 100; i <= 3000; i += 100) sizes.push_back(i);

        ofstream out("benchmark_results.csv");
        out << "Algorithm,DataType,Size,Time_us,Comparisons\n";

        auto run_test = [&](const string& name, auto sort_func, const string& d_type, const vector<string>& data) {
            long long total_time = 0;
            long long total_comps = 0;
            const int RUNS = 5;

            for (int r = 0; r < RUNS; ++r) {
                vector<string> arr = data;
                char_comparisons = 0;

                auto start = high_resolution_clock::now();
                sort_func(arr);
                auto end = high_resolution_clock::now();

                total_time += duration_cast<microseconds>(end - start).count();
                total_comps += char_comparisons;
            }
            out << name << "," << d_type << "," << data.size() << ","
                << total_time / RUNS << "," << total_comps / RUNS << "\n";
            cout << "Done: " << left << setw(20) << name << " | " << setw(15) << d_type << " | N=" << data.size() << "\n";
            };

        cout << "Generating test data (Size = 3000)..." << endl;
        vector<string> max_rand = gen.generateRandom(3000);
        vector<string> max_rev = gen.generateReverseSorted(3000);
        vector<string> max_near = gen.generateNearlySorted(3000);
        cout << "Data generated. Starting benchmarks..." << endl;

        for (int sz : sizes) {
            vector<string> rand_sub(max_rand.begin(), max_rand.begin() + sz);
            vector<string> rev_sub(max_rev.begin(), max_rev.begin() + sz);
            vector<string> near_sub(max_near.begin(), max_near.begin() + sz);

            vector<pair<string, vector<string>>> datasets = {
                {"Random", rand_sub}, {"ReverseSorted", rev_sub}, {"NearlySorted", near_sub}
            };

            for (auto& ds : datasets) {
                const string& type = ds.first;
                const vector<string>& data = ds.second;

                run_test("Std_QuickSort", [](vector<string>& a) { std_quicksort(a, 0, a.size() - 1); }, type, data);
                run_test("Std_MergeSort", [](vector<string>& a) { std_mergesort(a, 0, a.size() - 1); }, type, data);

                run_test("String_QuickSort", [](vector<string>& a) { string_quicksort(a, 0, a.size() - 1, 0); }, type, data);
                run_test("String_MergeSort", [](vector<string>& a) { string_mergesort(a); }, type, data);

                vector<string> aux(sz);
                run_test("MSD_Radix_NoFall", [&](vector<string>& a) { msd_radix(a, 0, a.size() - 1, 0, aux); }, type, data);
                run_test("MSD_Radix_Fallback", [&](vector<string>& a) { msd_radix_fallback(a, 0, a.size() - 1, 0, aux); }, type, data);
            }
        }

        out.close();
        cout << "\nBenchmarks completed. Results saved to benchmark_results.csv" << endl;
    }
};

int main() {
    StringSortTester tester;
    tester.runExperiments();
}
