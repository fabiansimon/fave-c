#ifndef DEBUG_H
#define DEBUG_H

void disassembleChunk(Chunk *chunk, const char *name);
void disassembleInstruction(Chunk *chunk, int *offset);

#endif