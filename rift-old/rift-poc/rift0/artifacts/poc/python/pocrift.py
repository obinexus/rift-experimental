from dataclasses import dataclass
import re
from typing import Dict, Set, Optional, List

@dataclass(frozen=True)
class State:
    """Immutable automaton state represented by a regex pattern."""
    pattern: str
    is_final: bool = False
    
    def matches(self, text: str) -> bool:
        return bool(re.match(self.pattern, text))

class RegexAutomaton:
    """Language-agnostic automaton using regex states."""
    
    def __init__(self):
        self.states: Set[State] = set()
        self.transitions: Dict[tuple[State, str], State] = {}
        self.initial_state: Optional[State] = None
        self.current_state: Optional[State] = None
        
    def add_state(self, pattern: str, is_final: bool = False) -> State:
        """Add a new regex state to the automaton."""
        state = State(pattern, is_final)
        self.states.add(state)
        if not self.initial_state:
            self.initial_state = state
            self.current_state = state
        return state
        
    def add_transition(self, from_state: State, input_pattern: str, to_state: State):
        """Add a transition between states."""
        self.transitions[(from_state, input_pattern)] = to_state
    
    def get_next_state(self, input_text: str) -> Optional[State]:
        """Find next valid state for given input."""
        if not self.current_state:
            return None
            
        # Try to match the current input directly
        for state in self.states:
            if state.matches(input_text):
                self.current_state = state
                return state
        return None

class IRNode:
    """Base class for IR nodes."""
    pass

@dataclass
class TokenNode(IRNode):
    """IR node representing a token."""
    type: str
    value: str

class IRGenerator:
    """Generates IR from automaton state sequences."""
    
    def __init__(self, automaton: RegexAutomaton):
        self.automaton = automaton
        self.ir_nodes: List[IRNode] = []
    
    def process_token(self, token: str) -> Optional[TokenNode]:
        """Process a token and generate corresponding IR."""
        next_state = self.automaton.get_next_state(token)
        if next_state:
            self.automaton.current_state = next_state
            return TokenNode(next_state.pattern, token)
        return None
    
    def generate_ir(self, tokens: List[str]) -> List[IRNode]:
        """Generate IR from a sequence of tokens."""
        self.ir_nodes = []
        for token in tokens:
            node = self.process_token(token)
            if node:
                self.ir_nodes.append(node)
        return self.ir_nodes

def create_simple_lexer():
    """Create a simple language-agnostic lexer."""
    automaton = RegexAutomaton()
    
    # Define regex states more precisely
    identifier = automaton.add_state(r'^[a-zA-Z_]\w*$')
    number = automaton.add_state(r'^\d+$')
    operator = automaton.add_state(r'^[+\-*/]$')
    whitespace = automaton.add_state(r'^\s+$')
    
    # Add states to set
    automaton.states.update({identifier, number, operator, whitespace})
    
    return automaton

if __name__ == "__main__":
    automaton = create_simple_lexer()
    generator = IRGenerator(automaton)
    
    # Test with a simple expression
    tokens = ["x", "+", "123", "*", "y"]
    ir = generator.generate_ir(tokens)
    
    print("Generated IR nodes:")
    for node in ir:
        print(f"Type: {node.type}, Value: {node.value}")