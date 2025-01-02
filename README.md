# Advent Of Code

Solutions to (some) of the [Advent of Code](https://adventofcode.com/) challenges.

Year | 2021 | 2022 | 2023 | 2024
--- | --- | --- | --- |--- 
Status | ONGOING | ONGOING | ONGOING | ONGOING

Mainly in C, Rust, and sometimes Python.

My main focuses when solving the challenges:

- **Readability**: I try to make the code as readable as possible. I want to be able to understand what I did when I come back to it in a few months.
- **Performance**: I try to make the code reasonably fast. Some solutions can takes a few seconds (some more than a minute IIRC) but that's OK.
- **Assumptions**: Sometimes some assumptions are made based on the provided input (e.g.: grid dimensions), this goes hand in hand with the laziness point below.
- **Error handling**: I don't always handle errors. I assume the code executes in a way that makes unknown errors impossible. For example, I don't necessarily check the return value of ` fopen()` calls to read input files. However, the embedded software engineer in me always checks for potential memory leaks (especially for C programs).
- **Laziness** : The solutions are not always state of the art. Sometimes I'll forget to free memory, or I'll use a suboptimal algorithm. Other times I'll just use a third-party library to solve the problem, this is especially true for Rust solutions (and especially for path finding problems).
