"""RIFT Stage 0 Tokenizer

This module provides a very small tokenizer based on the RIFT Stage 0
specification. It exposes a :func:`tokenize` function that converts source
strings into a sequence of tokens. The rules are taken from the language
specification and use Python regular expressions for matching.
"""

from dataclasses import dataclass
import re
from typing import List, Tuple


@dataclass
class Token:
    """Simple token data structure."""

    type: str
    value: str
    start: int
    end: int


class Tokenizer:
    """Compile regular expressions and tokenize RIFT source code."""

    def __init__(self) -> None:
        # Order matters: governance and memory references should be matched
        # before generic identifiers to avoid premature matches.
        patterns = [
            ("GOVERNANCE", r"governance\s*\{[^}]*\}"),
            ("MEMORY_REF", r"@[a-zA-Z_][a-zA-Z0-9_]*"),
            ("FUNC_SIG", r"\w+\s*\([^)]*\)"),
            ("EXPR_BLOCK", r"\{[^}]*\}"),
            ("STRING", r"\"([^\"\\]|\\.)*\""),
            ("NUMBER", r"\d+(\.\d+)?([eE][+-]?\d+)?"),
            ("IDENTIFIER", r"[a-zA-Z_][a-zA-Z0-9_]*"),
        ]
        self.regex = re.compile(
            "|".join(f"(?P<{name}>{pattern})" for name, pattern in patterns)
        )

    def tokenize(self, text: str) -> List[Token]:
        tokens: List[Token] = []
        pos = 0
        while pos < len(text):
            match = self.regex.match(text, pos)
            if not match:
                # Skip unknown character
                pos += 1
                continue
            typ = match.lastgroup
            value = match.group(typ)
            tokens.append(Token(typ, value, match.start(), match.end()))
            pos = match.end()
        return tokens


def tokenize(text: str) -> List[Token]:
    """Tokenize a text string using the default tokenizer."""

    return Tokenizer().tokenize(text)
