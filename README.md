# Hash Table from Scratch in C++ (Built, Broken, and Fixed)

A deep dive into how hash tables actually work — by building one from scratch, breaking its performance, and fixing it using real-world techniques.

---

## Why This Project?

Hash tables are often described as **O(1)** data structures.

But that’s not always true.

In this project, I explore:
- What makes hash tables fast
- What causes them to slow down
- How real systems maintain performance

---

## Features

- ✅ Generic implementation using templates (`<K, V>`)
- ✅ Collision handling using **chaining (linked lists)**
- ✅ Dynamic resizing (**rehashing**) when load factor exceeds threshold
- ✅ Prime number bucket sizing for better distribution
- ✅ Insert, Search, Delete operations
- ✅ Longest chain tracking (to observe worst-case performance)

---

## Key Concepts

### Hash Function
Maps a key to an index: