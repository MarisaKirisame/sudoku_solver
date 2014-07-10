sudoku_solver
=============
As it's name suggest, this is yet another sudoku solver.
Comparing to other sudoku solver, this is:
Lightweight. The source code is no more than 200 line, including an example.
Portable. It only rely on standard C++ and some boost header-only library.
Quick. It can solve http://www.telegraph.co.uk/science/science-news/9359579/Worlds-hardest-sudoku-can-you-crack-it.html in less than a second.
Customizable. It use iterator to return all answer, so user can use the result right after a single computation, either via boost::coroutine or other iterator/self-implemented ones.
