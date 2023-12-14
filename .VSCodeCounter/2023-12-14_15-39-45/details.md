# Details

Date : 2023-12-14 15:39:45

Directory e:\\ProgramCode\\Project\\BUAA_Compiler\\src

Total : 84 files,  7700 codes, 1111 comments, 1161 blanks, all 9972 lines

[Summary](results.md) / Details / [Diff Summary](diff.md) / [Diff Details](diff-details.md)

## Files
| filename | language | code | comment | blank | total |
| :--- | :--- | ---: | ---: | ---: | ---: |
| [src/Backend/Instruction.cpp](/src/Backend/Instruction.cpp) | C++ | 78 | 8 | 19 | 105 |
| [src/Backend/Instruction.h](/src/Backend/Instruction.h) | C++ | 175 | 12 | 24 | 211 |
| [src/Backend/MipsGenerator.cpp](/src/Backend/MipsGenerator.cpp) | C++ | 563 | 48 | 55 | 666 |
| [src/Backend/MipsGenerator.h](/src/Backend/MipsGenerator.h) | C++ | 61 | 5 | 24 | 90 |
| [src/Backend/Optimization/ColorAllocator.cpp](/src/Backend/Optimization/ColorAllocator.cpp) | C++ | 188 | 54 | 27 | 269 |
| [src/Backend/Optimization/ColorAllocator.h](/src/Backend/Optimization/ColorAllocator.h) | C++ | 40 | 7 | 7 | 54 |
| [src/Backend/Optimization/GraphNode.cpp](/src/Backend/Optimization/GraphNode.cpp) | C++ | 20 | 4 | 5 | 29 |
| [src/Backend/Optimization/GraphNode.h](/src/Backend/Optimization/GraphNode.h) | C++ | 17 | 4 | 6 | 27 |
| [src/Backend/Optimization/MulAndDiv.cpp](/src/Backend/Optimization/MulAndDiv.cpp) | C++ | 101 | 3 | 5 | 109 |
| [src/Backend/Optimization/MulAndDiv.h](/src/Backend/Optimization/MulAndDiv.h) | C++ | 14 | 3 | 4 | 21 |
| [src/Backend/Optimization/PeepHole.cpp](/src/Backend/Optimization/PeepHole.cpp) | C++ | 185 | 18 | 11 | 214 |
| [src/Backend/Optimization/PeepHole.h](/src/Backend/Optimization/PeepHole.h) | C++ | 18 | 3 | 6 | 27 |
| [src/Backend/Register/Register.cpp](/src/Backend/Register/Register.cpp) | C++ | 33 | 3 | 4 | 40 |
| [src/Backend/Register/Register.h](/src/Backend/Register/Register.h) | C++ | 26 | 3 | 8 | 37 |
| [src/Backend/Register/RegisterAlloc.cpp](/src/Backend/Register/RegisterAlloc.cpp) | C++ | 296 | 196 | 35 | 527 |
| [src/Backend/Register/RegisterAlloc.h](/src/Backend/Register/RegisterAlloc.h) | C++ | 90 | 19 | 28 | 137 |
| [src/Backend/Register/Registers.cpp](/src/Backend/Register/Registers.cpp) | C++ | 13 | 3 | 2 | 18 |
| [src/Backend/Register/Registers.h](/src/Backend/Register/Registers.h) | C++ | 13 | 3 | 4 | 20 |
| [src/Fronted/Error/Error.h](/src/Fronted/Error/Error.h) | C++ | 29 | 3 | 6 | 38 |
| [src/Fronted/Error/ErrorTable.cpp](/src/Fronted/Error/ErrorTable.cpp) | C++ | 19 | 3 | 7 | 29 |
| [src/Fronted/Error/ErrorTable.h](/src/Fronted/Error/ErrorTable.h) | C++ | 19 | 3 | 8 | 30 |
| [src/Fronted/Lexer/Lexer.cpp](/src/Fronted/Lexer/Lexer.cpp) | C++ | 173 | 3 | 16 | 192 |
| [src/Fronted/Lexer/Lexer.h](/src/Fronted/Lexer/Lexer.h) | C++ | 24 | 3 | 6 | 33 |
| [src/Fronted/Lexer/Token.cpp](/src/Fronted/Lexer/Token.cpp) | C++ | 176 | 13 | 10 | 199 |
| [src/Fronted/Lexer/Token.h](/src/Fronted/Lexer/Token.h) | C++ | 42 | 11 | 6 | 59 |
| [src/Fronted/Parser/AST.cpp](/src/Fronted/Parser/AST.cpp) | C++ | 522 | 6 | 55 | 583 |
| [src/Fronted/Parser/AST.h](/src/Fronted/Parser/AST.h) | C++ | 325 | 3 | 49 | 377 |
| [src/Fronted/Parser/Parser.cpp](/src/Fronted/Parser/Parser.cpp) | C++ | 529 | 52 | 73 | 654 |
| [src/Fronted/Parser/Parser.h](/src/Fronted/Parser/Parser.h) | C++ | 59 | 3 | 52 | 114 |
| [src/Middle/Intermediate/Immediate.cpp](/src/Middle/Intermediate/Immediate.cpp) | C++ | 8 | 3 | 2 | 13 |
| [src/Middle/Intermediate/Immediate.h](/src/Middle/Intermediate/Immediate.h) | C++ | 17 | 3 | 5 | 25 |
| [src/Middle/Intermediate/Intermediate.h](/src/Middle/Intermediate/Intermediate.h) | C++ | 8 | 3 | 3 | 14 |
| [src/Middle/MiddleCode.cpp](/src/Middle/MiddleCode.cpp) | C++ | 45 | 3 | 12 | 60 |
| [src/Middle/MiddleCode.h](/src/Middle/MiddleCode.h) | C++ | 25 | 4 | 10 | 39 |
| [src/Middle/MiddleCodeItem/BasicBlock.cpp](/src/Middle/MiddleCodeItem/BasicBlock.cpp) | C++ | 341 | 38 | 19 | 398 |
| [src/Middle/MiddleCodeItem/BasicBlock.h](/src/Middle/MiddleCodeItem/BasicBlock.h) | C++ | 42 | 5 | 13 | 60 |
| [src/Middle/MiddleCodeItem/Func.cpp](/src/Middle/MiddleCodeItem/Func.cpp) | C++ | 45 | 4 | 2 | 51 |
| [src/Middle/MiddleCodeItem/Func.h](/src/Middle/MiddleCodeItem/Func.h) | C++ | 39 | 4 | 8 | 51 |
| [src/Middle/MiddleCodeItem/Label.cpp](/src/Middle/MiddleCodeItem/Label.cpp) | C++ | 6 | 3 | 1 | 10 |
| [src/Middle/MiddleCodeItem/Label.h](/src/Middle/MiddleCodeItem/Label.h) | C++ | 14 | 3 | 5 | 22 |
| [src/Middle/MiddleCodeItem/Middle.h](/src/Middle/MiddleCodeItem/Middle.h) | C++ | 16 | 4 | 3 | 23 |
| [src/Middle/MiddleCodeItem/MiddleBinaryOp.cpp](/src/Middle/MiddleCodeItem/MiddleBinaryOp.cpp) | C++ | 26 | 4 | 6 | 36 |
| [src/Middle/MiddleCodeItem/MiddleBinaryOp.h](/src/Middle/MiddleCodeItem/MiddleBinaryOp.h) | C++ | 35 | 3 | 5 | 43 |
| [src/Middle/MiddleCodeItem/MiddleCodeItem.cpp](/src/Middle/MiddleCodeItem/MiddleCodeItem.cpp) | C++ | 113 | 16 | 20 | 149 |
| [src/Middle/MiddleCodeItem/MiddleCodeItem.h](/src/Middle/MiddleCodeItem/MiddleCodeItem.h) | C++ | 56 | 3 | 13 | 72 |
| [src/Middle/MiddleCodeItem/MiddleDef.cpp](/src/Middle/MiddleCodeItem/MiddleDef.cpp) | C++ | 24 | 4 | 5 | 33 |
| [src/Middle/MiddleCodeItem/MiddleDef.h](/src/Middle/MiddleCodeItem/MiddleDef.h) | C++ | 24 | 5 | 6 | 35 |
| [src/Middle/MiddleCodeItem/MiddleFuncCall.cpp](/src/Middle/MiddleCodeItem/MiddleFuncCall.cpp) | C++ | 9 | 4 | 1 | 14 |
| [src/Middle/MiddleCodeItem/MiddleFuncCall.h](/src/Middle/MiddleCodeItem/MiddleFuncCall.h) | C++ | 12 | 3 | 3 | 18 |
| [src/Middle/MiddleCodeItem/MiddleIO.cpp](/src/Middle/MiddleCodeItem/MiddleIO.cpp) | C++ | 32 | 4 | 6 | 42 |
| [src/Middle/MiddleCodeItem/MiddleIO.h](/src/Middle/MiddleCodeItem/MiddleIO.h) | C++ | 21 | 3 | 5 | 29 |
| [src/Middle/MiddleCodeItem/MiddleJump.cpp](/src/Middle/MiddleCodeItem/MiddleJump.cpp) | C++ | 21 | 4 | 5 | 30 |
| [src/Middle/MiddleCodeItem/MiddleJump.h](/src/Middle/MiddleCodeItem/MiddleJump.h) | C++ | 27 | 5 | 3 | 35 |
| [src/Middle/MiddleCodeItem/MiddleMemoryOp.cpp](/src/Middle/MiddleCodeItem/MiddleMemoryOp.cpp) | C++ | 39 | 4 | 4 | 47 |
| [src/Middle/MiddleCodeItem/MiddleMemoryOp.h](/src/Middle/MiddleCodeItem/MiddleMemoryOp.h) | C++ | 21 | 4 | 2 | 27 |
| [src/Middle/MiddleCodeItem/MiddleOffset.cpp](/src/Middle/MiddleCodeItem/MiddleOffset.cpp) | C++ | 23 | 8 | 8 | 39 |
| [src/Middle/MiddleCodeItem/MiddleOffset.h](/src/Middle/MiddleCodeItem/MiddleOffset.h) | C++ | 19 | 3 | 3 | 25 |
| [src/Middle/MiddleCodeItem/MiddleReturn.cpp](/src/Middle/MiddleCodeItem/MiddleReturn.cpp) | C++ | 14 | 4 | 4 | 22 |
| [src/Middle/MiddleCodeItem/MiddleReturn.h](/src/Middle/MiddleCodeItem/MiddleReturn.h) | C++ | 13 | 3 | 3 | 19 |
| [src/Middle/MiddleCodeItem/MiddleUnaryOp.cpp](/src/Middle/MiddleCodeItem/MiddleUnaryOp.cpp) | C++ | 19 | 4 | 6 | 29 |
| [src/Middle/MiddleCodeItem/MiddleUnaryOp.h](/src/Middle/MiddleCodeItem/MiddleUnaryOp.h) | C++ | 25 | 8 | 4 | 37 |
| [src/Middle/MiddleCodeItem/PushParam.cpp](/src/Middle/MiddleCodeItem/PushParam.cpp) | C++ | 11 | 8 | 3 | 22 |
| [src/Middle/MiddleCodeItem/PushParam.h](/src/Middle/MiddleCodeItem/PushParam.h) | C++ | 12 | 3 | 4 | 19 |
| [src/Middle/Optimization/DataFlow.cpp](/src/Middle/Optimization/DataFlow.cpp) | C++ | 537 | 40 | 40 | 617 |
| [src/Middle/Optimization/DataFlow.h](/src/Middle/Optimization/DataFlow.h) | C++ | 47 | 8 | 27 | 82 |
| [src/Middle/Optimization/DefinitionSet.cpp](/src/Middle/Optimization/DefinitionSet.cpp) | C++ | 57 | 3 | 7 | 67 |
| [src/Middle/Optimization/DefinitionSet.h](/src/Middle/Optimization/DefinitionSet.h) | C++ | 25 | 4 | 10 | 39 |
| [src/Middle/Optimization/NoChangeValue.cpp](/src/Middle/Optimization/NoChangeValue.cpp) | C++ | 102 | 3 | 5 | 110 |
| [src/Middle/Optimization/NoChangeValue.h](/src/Middle/Optimization/NoChangeValue.h) | C++ | 22 | 3 | 6 | 31 |
| [src/Middle/Optimization/PositiveSet.cpp](/src/Middle/Optimization/PositiveSet.cpp) | C++ | 23 | 3 | 7 | 33 |
| [src/Middle/Optimization/PositiveSet.h](/src/Middle/Optimization/PositiveSet.h) | C++ | 21 | 3 | 13 | 37 |
| [src/Middle/Symbol/Symbol.cpp](/src/Middle/Symbol/Symbol.cpp) | C++ | 65 | 3 | 19 | 87 |
| [src/Middle/Symbol/Symbol.h](/src/Middle/Symbol/Symbol.h) | C++ | 96 | 11 | 11 | 118 |
| [src/Middle/Symbol/SymbolTable.cpp](/src/Middle/Symbol/SymbolTable.cpp) | C++ | 66 | 9 | 11 | 86 |
| [src/Middle/Symbol/SymbolTable.h](/src/Middle/Symbol/SymbolTable.h) | C++ | 23 | 5 | 12 | 40 |
| [src/Middle/Util/Calculate.cpp](/src/Middle/Util/Calculate.cpp) | C++ | 136 | 6 | 22 | 164 |
| [src/Middle/Util/Calculate.h](/src/Middle/Util/Calculate.h) | C++ | 13 | 3 | 11 | 27 |
| [src/Middle/Util/Flatten.h](/src/Middle/Util/Flatten.h) | C++ | 6 | 3 | 4 | 13 |
| [src/Middle/Visitor/Visitor.cpp](/src/Middle/Visitor/Visitor.cpp) | C++ | 1,252 | 286 | 119 | 1,657 |
| [src/Middle/Visitor/Visitor.h](/src/Middle/Visitor/Visitor.h) | C++ | 74 | 3 | 52 | 129 |
| [src/Optimization/Config.h](/src/Optimization/Config.h) | C++ | 9 | 16 | 3 | 28 |
| [src/Util/Debug.h](/src/Util/Debug.h) | C++ | 15 | 7 | 8 | 30 |
| [src/Util/Exception.h](/src/Util/Exception.h) | C++ | 10 | 3 | 5 | 18 |
| [src/main.cpp](/src/main.cpp) | C++ | 51 | 6 | 5 | 62 |

[Summary](results.md) / Details / [Diff Summary](diff.md) / [Diff Details](diff-details.md)