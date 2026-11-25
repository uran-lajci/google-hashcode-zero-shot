import unittest

from harness import config
from harness.programs import code_blocks, extract


def message(text: str, stop_reason: str = 'end_turn') -> dict:
    return {
        'content': [{'type': 'thinking', 'thinking': ''}, {'type': 'text', 'text': text}],
        'stop_reason': stop_reason,
    }


class TestCodeBlocks(unittest.TestCase):
    def test_one_block(self) -> None:
        self.assertEqual(code_blocks('Text.\n\n```cpp\nint main() {}\n```\n\nMore text.'), ['int main() {}\n'])

    def test_block_keeps_blank_lines_and_indentation(self) -> None:
        self.assertEqual(code_blocks('```cpp\nint main() {\n\n    return 0;\n}\n```'), ['int main() {\n\n    return 0;\n}\n'])

    def test_no_block(self) -> None:
        self.assertEqual(code_blocks('I use `std::sort` here.'), [])

    def test_block_that_is_never_closed(self) -> None:
        self.assertEqual(code_blocks('```cpp\nint main() {'), [])

    def test_several_blocks(self) -> None:
        self.assertEqual(code_blocks('```bash\nmake\n```\ntext\n```\nint x;\n```'), ['make\n', 'int x;\n'])

    def test_longer_fence_holds_a_shorter_one(self) -> None:
        self.assertEqual(code_blocks('````cpp\nauto s = R"(\n```\n)";\n````'), ['auto s = R"(\n```\n)";\n'])

    def test_tilde_fence(self) -> None:
        self.assertEqual(code_blocks('~~~cpp\nint x;\n~~~'), ['int x;\n'])

    def test_windows_line_ends(self) -> None:
        self.assertEqual(code_blocks('```cpp\r\nint x;\r\n```\r\n'), ['int x;\r\n'])


class TestExtract(unittest.TestCase):
    def test_program(self) -> None:
        result = extract(message('```cpp\nint main() {}\n```'))
        self.assertEqual((result['state'], result['source'], result['flagged']), ('program', 'int main() {}\n', False))

    def test_longest_block_is_the_program_and_the_reply_is_flagged(self) -> None:
        result = extract(message('```\ng++ a.cpp\n```\n```cpp\nint main() { return 0; }\n```'))
        self.assertEqual((result['source'], result['code_blocks'], result['flagged']), ('int main() { return 0; }\n', 2, True))

    def test_no_code_block(self) -> None:
        result = extract(message('I cannot solve this.'))
        self.assertEqual((result['state'], result['reason']), ('no_program', 'no code block'))

    def test_cut_off_reply_has_no_program_even_with_a_closed_block(self) -> None:
        result = extract(message('```cpp\nint main() {}\n```\nThe idea is', stop_reason='max_tokens'))
        self.assertEqual((result['state'], result['reason']), ('no_program', 'cut off at the output limit'))
        self.assertNotIn('source', result)

    def test_refusal(self) -> None:
        result = extract({'content': [], 'stop_reason': 'refusal'})
        self.assertEqual((result['state'], result['reason']), ('no_program', 'refusal'))


class TestProtocolValues(unittest.TestCase):
    def test_prompt_is_the_block_of_the_protocol(self) -> None:
        prompt = config.prompt()
        self.assertTrue(prompt.startswith('The attached PDF is the problem statement of a Google Hash Code problem.\n'))
        self.assertTrue(prompt.endswith('Reply with the program in one ```cpp code block.'))
        self.assertIn('7. It will be compiled with: g++ -O2 -std=c++17', prompt)

    def test_number_of_generations(self) -> None:
        self.assertEqual(len(config.STAGES['pilot'].programs()), 6)
        self.assertEqual(len(config.STAGES['study'].programs()), 510)
        study = config.STAGES['study']
        self.assertEqual(sum(len(study.instances(problem)) for problem in study.problems()), 76)


if __name__ == '__main__':
    unittest.main()
