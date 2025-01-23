#include <bits/stdc++.h>
using namespace std;

/*
  Structura unui nod de Trie:
  - children: fișiere care leagă un caracter de un nod următor
  - terminal: marchează sfârșitul (cel puțin) unui cuvânt acceptat
*/
struct TrieNode {
    bool terminal;
    // Pentru simplificare, presupunem caractere ASCII posibile.
    // Dacă știi un alfabet limitat (ex. 'a'..'z'), poți folosi un vector<TrieNode*> de 26.
    unordered_map<char, TrieNode*> children;

    TrieNode() : terminal(false) {}
};

// Inserăm un cuvânt în Trie
void insertWord(TrieNode* root, const string &word) {
    TrieNode* current = root;
    for (char c : word) {
        if (!current->children.count(c)) {
            current->children[c] = new TrieNode();
        }
        current = current->children[c];
    }
    // Marcam sfarsit de cuvant
    current->terminal = true;
}

// Verificăm dacă un cuvânt (word) este recunoscut de Trie (din root)
bool recognizedByTrie(TrieNode* root, const string &word) {
    TrieNode* current = root;
    for (char c : word) {
        if (!current->children.count(c)) {
            return false;  // nu mai există ramură pentru c
        }
        current = current->children[c];
    }
    // Cuvântul este recunoscut dacă ultimul nod este terminal
    return current->terminal;
}

/*
  Funcție care verifică dacă dintr-un nod dat se poate ajunge la un nod terminal
  (dacă există o cale până la final).
*/
bool canReachTerminal(TrieNode* node) {
    if (node->terminal) return true;
    for (auto &kv : node->children) {
        if (canReachTerminal(kv.second)) {
            return true;
        }
    }
    return false;
}

/*
  Construim un fel de "regex" dintr-un nod de Trie, cu modificări:
  1. Folosim "wildcarduri" (de fapt grupări de caractere) pentru subseturi de litere care duc la același subpattern:
     - Dacă mai multe ramuri au același sub-regex, le combinăm într-un [charClass] + subpattern.
  2. Păstrăm prefixele comune o singură dată (prin combinarea ramurilor cu aceeași continuare).
  3. Dacă dintr-un copil se poate ajunge în final, punem `.*` după subpattern, semnificând "oricâte wildcarduri".
*/
string buildRegexFromTrie(TrieNode* node) {
    // Dacă nu are copii, poate fi doar un terminal izolat.
    // Reprezentăm prin string gol (s-a terminat).
    // Dar dacă e terminal, înseamnă că aici se poate încheia cuvântul.
    if (node->children.empty()) {
        // Dacă e nod terminal, întoarcem "", care va reprezenta capătul de acceptare.
        return "";
    }

    // Reținem dacă nodul curent poate închide un cuvânt (terminal).
    bool hasTerminalOption = node->terminal;

    /*
      Ne vom organiza copiii în funcție de subRegex (continuarea):
      patternMap[subRegex] = listă de caractere care duc la acest subRegex.
      Ulterior, vom combina caracterele într-un bracket [abc], urmat de subRegex.
    */
    unordered_map<string, vector<char>> patternMap;

    // Pentru fiecare copil, generăm recursiv subRegex-ul lui.
    for (auto &kv : node->children) {
        char c = kv.first;
        TrieNode* child = kv.second;
        // Construim subRegex pentru copil (recursiv)
        string childRegex = buildRegexFromTrie(child);

        // 3) Dacă din copil se poate ajunge la un nod terminal, adăugăm ".*"
        //    ca să semnifice "orice urmează" (oricâte wildcarduri).
        if (canReachTerminal(child)) {
            childRegex += ".*";
        }

        // Înregistrăm caracterul c pentru subRegex-ul childRegex în patternMap.
        patternMap[childRegex].push_back(c);
    }

    // Acum construim ramurile (branches) pentru nodul curent.
    vector<string> branches;
    branches.reserve(patternMap.size());

    for (auto &entry : patternMap) {
        const string &sub = entry.first;       // subRegex
        vector<char> &chars = entry.second;    // caractere care duc la același subRegex

        // Sortăm și eliminăm duplicatele pentru a construi corect clasa de caractere
        sort(chars.begin(), chars.end());
        chars.erase(unique(chars.begin(), chars.end()), chars.end());

        // Dacă avem un singur caracter, îl punem direct,
        // altfel îl punem între [ ].
        string charClass;
        if (chars.size() == 1) {
            charClass = string(1, chars[0]);
        } else {
            charClass = "[";
            for (char c : chars) {
                charClass.push_back(c);
            }
            charClass += "]";
        }

        // Dacă subRegex conține '|' (branching), e posibil să avem nevoie de paranteze.
        bool needParens = (sub.find('|') != string::npos);

        // Construcția finală pentru această ramură:
        // charClass + (sub) dacă sub nu e gol,
        // iar dacă sub conține '|', îl încapsulăm în paranteze.
        string branchStr;
        if (!sub.empty()) {
            if (needParens) {
                branchStr = charClass + "(" + sub + ")";
            } else {
                branchStr = charClass + sub;
            }
        } else {
            // sub e gol, deci ramura e doar charClass
            branchStr = charClass;
        }

        branches.push_back(branchStr);
    }

    // Dacă avem mai multe ramuri, le unim cu '|'. Dacă avem una singură, rămâne așa.
    string combined;
    if (branches.size() == 1) {
        combined = branches[0];
    } else {
        combined = "(";
        for (int i = 0; i < (int)branches.size(); i++) {
            if (i > 0) combined += "|";
            combined += branches[i];
        }
        combined += ")";
    }

    // Dacă nodul curent este terminal, avem opțiunea să ne oprim aici (""), așa că facem OR cu "()".
    if (hasTerminalOption) {
        if (combined.empty()) {
            return "()"; // dacă n-avem copii, doar "()"
        } else {
            return "(" + combined + "|()" + ")";
        }
    }

    return combined;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ifstream fin("input.txt");
    ofstream fout("output.txt");

    int cntAccept, cntFail, lenString;
    fin >> cntAccept >> cntFail >> lenString;

    vector<string> acceptWords(cntAccept);
    for(int i = 0; i < cntAccept; i++){
        fin >> acceptWords[i];
    }
    vector<string> failWords(cntFail);
    for(int i = 0; i < cntFail; i++){
        fin >> failWords[i];
    }

    // Caz trivial: nimic in Accept => nu acceptam nimic
    if (cntAccept == 0) {
        fout << "regex\n()\n";
        return 0;
    }

    // Construim un Trie cu toate cuvintele acceptate
    TrieNode* root = new TrieNode();
    for (auto &w : acceptWords) {
        insertWord(root, w);
    }

    // Generăm patternul (pseudo-regex) din Trie
    string trieRegex = buildRegexFromTrie(root);

    // Verificăm dacă vreo failWord este recunoscută de Trie:
    bool conflict = false;
    for (auto &fw : failWords) {
        if (recognizedByTrie(root, fw)) {
            // Avem un conflict: un cuvânt respins e recunoscut de Trie => regex prea permisiv
            conflict = true;
            break;
        }
    }

    // Dacă avem conflict, fallback la OR-ul simplu cu cuvintele acceptate
    fout << "regex\n";
    if (conflict) {
        // (w1|w2|...|wN)
        fout << "(";
        for (int i = 0; i < cntAccept; i++) {
            if (i > 0) fout << "|";
            fout << acceptWords[i];
        }
        fout << ")\n";
    } else {
        // Altfel, scriem trieRegex
        if (trieRegex.empty()) {
            fout << "()\n";
        } else {
            fout << trieRegex << "\n";
        }
    }

    // Eliberăm memoria (opțional, pentru siguranță) - omis în acest exemplu

    return 0;
}
