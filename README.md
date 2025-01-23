# Regex Generator README

This project provides an implementation of a tool that generates a regular expression (regex) pattern to classify words into two categories: accepted and rejected. The tool reads input from `input.txt` and writes the resulting regex to `output.txt`. The solution is implemented in Python and is designed to handle the requirements described in the problem statement efficiently.

---

## Implementation Details

The program is structured into several components, each handling a specific task. Here's an overview of the main parts of the implementation:

### 1. WordTree Class
The `WordTree` class represents a tree structure for storing and processing word patterns. Each node in the tree corresponds to a character and can have:
- `patterns`: A dictionary where each key is a character leading to a subtree.
- `is_terminal`: A flag indicating whether the current node marks the end of an accepted word.
- `blocked`: A flag marking whether the path corresponds to a rejected pattern.

### 2. Input Processing
The `process_text_file` function reads the input file and extracts:
- The list of accepted words.
- The list of rejected words.
- The fixed length of all words in the input.

### 3. Building the Word Tree
The `build_word_tree` function constructs the tree structure based on the accepted words. Each character of a word is added as a branch in the tree, ensuring that all accepted words are represented.

### 4. Handling Rejected Patterns
The `gather_blocked_patterns` function identifies all prefixes of the rejected words, ensuring that the regex avoids matching these patterns. The `mark_blocked_paths` function traverses the tree and marks nodes that correspond to blocked patterns.

### 5. Regex Generation
The `create_regex_pattern` function recursively generates the regex pattern based on the tree structure. Key features include:
- Using `.*` to represent unrestricted paths.
- Concatenating characters when there is only one child node.
- Forming groups with alternation (`|`) for multiple child nodes.

### 6. Main Function
The `main` function handles the overall workflow:
1. Reads the input file.
2. Builds the word tree for accepted words.
3. Marks paths corresponding to rejected patterns.
4. Generates the regex pattern.
5. Writes the regex pattern to the output file.

---

## How to Use

### Input Format
The program expects an `input.txt` file with the following structure:
1. The first line contains three integers:
   - `cnt_accept`: Number of accepted words.
   - `cnt_fail`: Number of rejected words.
   - `len_string`: Length of each word.
2. The next `cnt_accept` lines list the accepted words.
3. The following `cnt_fail` lines list the rejected words.

Example `input.txt`:
```
3 2 4
baed
bece
bace
badc
aace
```

### Output Format
The program writes the regex pattern to `output.txt` in the following format:
```
regex
<generated regex>
```

Example `output.txt`:
```
regex
b.(ed|le)e
```

### Running the Program
To execute the program:
1. Ensure `input.txt` exists in the same directory as the script.
2. Run the script using Python:
   ```bash
   python script_name.py
   ```
3. The generated regex will be saved in `output.txt`.

---

## Features
- Efficiently handles the classification of words using a tree-based structure.
- Generates precise regex patterns that avoid rejected words while accepting all valid ones.
- Supports words of fixed lengths with complex patterns.

---

## Limitations
- All words must have the same length as specified in the input file.
- The regex generation is designed for the specific structure of the problem and may not generalize to other regex tasks.

---

## File Overview
- **`script_name.py`**: The main Python script implementing the solution.
- **`input.txt`**: Input file containing accepted and rejected words.
- **`output.txt`**: Output file containing the generated regex.

---

## Example Workflow
1. Create an `input.txt` file with the required format.
2. Run the script.
3. Check the `output.txt` file for the generated regex.

---

## Author
This project was implemented with a focus on solving a classification problem using regular expressions. For any questions, feel free to open an issue on GitHub or contact the author directly.

