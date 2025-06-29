import unittest
from riftlib.tokenizer import tokenize


class TokenizerTest(unittest.TestCase):
    def test_basic_tokenization(self):
        source = (
            'governance { stage: 0 }\n'
            'function foo(bar) { return bar + 42 }\n'
            '@memory1\n'
            '"hello" 123'
        )
        tokens = tokenize(source)
        self.assertTrue(any(t.type == 'GOVERNANCE' for t in tokens))
        self.assertTrue(any(t.type == 'FUNC_SIG' for t in tokens))
        self.assertTrue(any(t.type == 'MEMORY_REF' for t in tokens))
        self.assertTrue(any(t.type == 'STRING' for t in tokens))
        self.assertTrue(any(t.type == 'NUMBER' for t in tokens))


if __name__ == '__main__':
    for token in tokenize('governance { a }'):
        print(token)
