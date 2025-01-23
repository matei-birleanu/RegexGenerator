import sys
from dataclasses import dataclass
from typing import Dict


# o clasa ce reprezinta un arbore de cuvinte
# un nod poate avea subnoduri corespunzatoare literelor ce urmeaza
@dataclass
class WordTree:
    patterns: Dict[str, 'WordTree'] # dictionar de copii unde fiecare litera este legata de un subarbore
    is_terminal: bool = False # marcheaza sfarsitul unui cuvant din lista de accept
    blocked: bool = False # calea din arbore se potrivesete cu un pattern respins
    
    @classmethod
    def create_empty(cls): # creez un nod gol pt arbore
        return cls(patterns={})

def process_text_file(filepath):
    with open(filepath, 'r') as file:
        counts = list(map(int, file.readline().split()))
        accept_list = [file.readline().strip() for _ in range(counts[0])]
        reject_list = [file.readline().strip() for _ in range(counts[1])]
        return accept_list, reject_list, counts[2]

def build_word_tree(sequences):
    if not sequences:
        return None
        
    root = WordTree.create_empty()
    for word in sequences:
        current = root
        # parcurg fiecare litera si construiesc ramuri in arbore
        for char in word:
            if char not in current.patterns:
                current.patterns[char] = WordTree.create_empty()
            current = current.patterns[char]
        current.is_terminal = True
    return root

def gather_blocked_patterns(words):
    patterns = set()
    for word in words:
        current = ''
        patterns.add(current)
        for char in word:
            current += char
            patterns.add(current)
    return patterns

def mark_blocked_paths(tree, current, blocked):
    # marchez nodurile care corespund cu pattern-uri respinse
    if current in blocked:
        tree.blocked = True
        
    for char, subtree in tree.patterns.items():
        mark_blocked_paths(subtree, current + char, blocked)

def create_regex_pattern(tree, depth, max_length):
    # construiesc recursiv regex pe baza arboerului
    if depth == max_length:
        return ""
        
    if not tree.blocked:
        return ".*"
        
    if not tree.patterns:
        # sfarsitul unei ramuri
        return "()"
    
    # exista un singur copil si continui concatenarea fara alternante
    if len(tree.patterns) == 1:
        char, next_node = next(iter(tree.patterns.items()))
        return char + create_regex_pattern(next_node, depth + 1, max_length)

    # creez un grup de alternative
    alternatives = []
    for char, sub_tree in sorted(tree.patterns.items()):
        pattern = create_regex_pattern(sub_tree, depth + 1, max_length)
        alternatives.append(f"{char}{pattern}")
    
    return f"({'|'.join(alternatives)})"

def generate_pattern(accept_words, reject_words,word_length):
    if not accept_words:
        return "()"
        
    # construiesc structura
    word_tree = build_word_tree(accept_words)
    if not word_tree:
        return "()"
        
    # colectez pattern-urile blocate
    blocked = gather_blocked_patterns(reject_words)
    # marchez pattern-urile care trebuie evitate
    mark_blocked_paths(word_tree, "", blocked)
    
    return create_regex_pattern(word_tree, 0, word_length)

def main():    
    input_file = "input.txt"
    output_file = "output.txt"
    accept_words, reject_words, length = process_text_file(input_file)
    pattern = generate_pattern(accept_words, reject_words, length)
    with open(output_file, 'w') as file:
        file.write("regex\n")
        file.write(f"{pattern}\n")

if __name__ == "__main__":
    main()