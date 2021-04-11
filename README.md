# io-reference
Potential reference solution to our homework in C++20. 
Definitely not the only one and no claims about being the 'best' one - the assignment is intentionally vague and short of details so you are expected to work them out with the assigner :)

This one tried to be reasonably efficient (the interface tries to prevent any unneeded memory allocations so no returning of vectors and such) and is modelled roughly according to the std::ios inheritance tree, all the way through to its virtual inheritance (yay!) - but without its more complex replaceable parts (the internal buffer). Enjoy!
