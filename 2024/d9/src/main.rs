#![allow(dead_code)]

#[derive(Clone, Copy, Debug, PartialEq)]
enum BlockType {
    FreeSpace,
    File(usize /* ID */),
}

#[derive(Clone, Copy, Debug, PartialEq)]
struct Block {
    size: usize,
    form: BlockType,
}

fn display(blocks: &Vec<Block>) {
    for b in blocks {
        match b.form {
            BlockType::FreeSpace => print!("{}", ".".repeat(b.size)),
            BlockType::File(id) => {
                for _ in 0..b.size {
                    let s = id.to_string();
                    print!("{}", s);
                }
            }
        }
    }
    println!();
}

#[inline(always)]
fn checksum(blocks: &Vec<Block>) -> usize {
    blocks
        .iter()
        .enumerate()
        .map(|(i, b)| match b.form {
            BlockType::FreeSpace => 0,
            BlockType::File(id) => i * id,
        })
        .sum()
}

fn compact(blocks: &mut Vec<Block>) -> usize {
    let mut blocks = flatten(blocks);
    while let Some(freespace) = blocks.iter().position(|b| b.form == BlockType::FreeSpace) {
        let index_of_diskfiles_after_first_freespace = blocks
            .iter()
            .enumerate()
            .filter(|(i, b)| *i > freespace && !(b.form == BlockType::FreeSpace))
            .map(|(i, _)| i)
            .last();

        if let Some(last) = index_of_diskfiles_after_first_freespace {
            blocks.swap(freespace, last);
            blocks.remove(last);
        } else {
            break;
        }
    }
    checksum(&blocks)
}

fn compact_p2(mut blocks: Vec<Block>) -> usize {
    let diskfile_blocks: Vec<Block> = blocks
        .iter()
        .filter(|b| b.form != BlockType::FreeSpace)
        .map(|b| *b)
        .rev()
        .collect::<Vec<Block>>();

    for block in diskfile_blocks {
        let idx = blocks.iter().position(|b| *b == block).unwrap();
        let size = blocks[idx].size;
        if let Some(first_eligible_freespace) = blocks
            .iter()
            .enumerate()
            .filter(|(i, b)| b.size >= size && b.form == BlockType::FreeSpace && *i < idx)
            .map(|(i, _)| i)
            .collect::<Vec<_>>()
            .first()
        {
            let new_freespace_size = blocks[*first_eligible_freespace].size - size;

            blocks[idx] = Block {
                size: block.size,
                form: BlockType::FreeSpace,
            };
            blocks[*first_eligible_freespace] = block;

            // the previous freespace is chunked, we must restore the remaining block of freespace
            if new_freespace_size > 0 {
                blocks.insert(
                    *first_eligible_freespace + 1,
                    Block {
                        size: new_freespace_size,
                        form: BlockType::FreeSpace,
                    },
                );
            }
        }
    }

    checksum(&flatten(&blocks))
}

fn flatten(blocks: &Vec<Block>) -> Vec<Block> {
    blocks
        .iter()
        .map(|b| {
            if b.size == 1 {
                vec![*b]
            } else {
                vec![
                    Block {
                        form: b.form,
                        size: 1
                    };
                    b.size
                ]
            }
        })
        .flatten()
        .collect::<Vec<Block>>()
}

fn main() {
    let args = std::env::args().collect::<Vec<String>>();
    let input = args.get(1).map_or("input_test", |v| v);

    let binding = std::fs::read_to_string(input).unwrap();
    let mut i = 0usize;
    let mut blocks: Vec<Block> = vec![];
    let mut file_count = -1isize;
    for c in binding.trim_end().chars() {
        let size = c.to_digit(10).unwrap() as usize;
        blocks.push(if i % 2 == 0 {
            file_count += 1;
            Block {
                size,
                form: BlockType::File(file_count as usize),
            }
        } else {
            Block {
                size,
                form: BlockType::FreeSpace,
            }
        });
        i += 1;
    }

    let part1 = compact(&mut blocks.clone());
    let part2 = compact_p2(blocks);
    println!("{part1:?} {part2:?} ");
}
