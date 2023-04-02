# McCuckoo

Cuckoo Hashing is a technique for implementing a hash table. Unlike most other hash tables, it achieves constant time worst-case complexity for lookups. However, insertion might lead to cycles that are tough to break out of and causes computation of the hash function multiple times. These problems can be handled using Multi-copy cuckoo hashing. 
Multi-Copy Cuckoo Hashing inserts copies of keys into multiple hash tables simultaneously, so when multiple candidate buckets are available, it is not necessary to randomly select a candidate bucket when inserting. This improves lookup time. However, the probability of rehashing stays the same; hence, an additional stash is used to store keys to temporarily delay the need for rehashing. But lookup in a big stash would increase the worst-case time complexity to linear,  and once an inserted item is deleted from the stash, it must again be traversed during a lookup. For this, a bloom filter-like structure is used to reduce the frequency of lookup in stash; however, this causes the frequency of false positives.

# Solution to McCuckoo

* Linear Probing in Stash:
A stash-based approach is implemented to store the items that failed to be inserted during the insertion process. While this method helps to avoid the costly rehashing operation, it comes at the cost of increased time for lookup.
To ensure that the lookup in the stash is constant, Linear Probing can be used. The hash function helps reduce the time complexity of lookup to constant.

* Flags- Counting Bloom Filter:
An array of flags will be made, which will work in a fashion similar to a counting bloom filter. A counting bloom filter is a modification of a bloom filter as it does not give any false negative results but also reduces the probability of false positives. In case of deletion, the counter value is decremented. This ensures that the false positive rate does not increase simultaneously, ensuring no false negatives are there.  

# Data Structures Used

* Off-chip hash table: The off-chip table is the hash table that stores the real items in the sub-tables. It stores the items according to the corresponding hash function for the table. The hash table is implemented using 2 fixed length one-dimensional integer arrays of size M. We use two independent hash functions for the two tables.

* On-chip counters: The on-chip counters contain the counters that are one-to-one mapped to the off-chip candidate buckets. The counters are initially set to 0 and are updated with each insertion and deletion. The on-chip counters are implemented using  2 fixed length one-dimensional short int type arrays of size M.

* Off-chip stash: A stash is a hash table that uses a simple hash function to store the items from the failed insertions into the off-chip table, to avoid costly rehashing. We have used a fixed-length one-dimensional integer array for a stash with linear probing of size S. A separate hash function is used to map keys in the stash. 

* Flags: The flags is an array that is initially set to 0 and work in the fashion of a counting-bloom filter, when an item fails in insertion and is put to the stash, the flags of its candidate buckets are incremented, so at a later time when we want to decide whether to check the stash, we will see first if any of the flags of the associated buckets are 0, if yes we know for sure it is not there without accessing the stash. The stash flags are assigned one-to-one to each candidate bucket. The flag table was implemented using 2 fixed length one-dimensional short int type arrays of size M.

<img width="264" alt="image" src="https://user-images.githubusercontent.com/118632573/229333610-7ec48402-0c18-4011-971f-6caf696f0304.png">

# Conclusion

Multicopy Cuckoo Hashing had problems in cases where Deletion in the Multicopy Cuckoo Hash Table was more frequently done. Our solution, the 
"Counting Bloom Filter" improves the performance of the Hash Tables in cases where Deletions are more frequent. Two examples where deletion is as frequent as insertion are:
1)	Active Covid Cases: Supposing the people with Covid are stored in the Hash tables. Since people recover within 14 days and assume a constant caseload, our solution can prove to be beneficial.
2)	Air Traffic Control Management: In busier airports where airplanes are to land in the airport, they can be stored in Hash Tables for faster lookup in case any communication is required. Once the airplanes land, they need to be removed from the table, so deletion will be as frequent as insertion and our solution can come in handy again. Finally, keys in the stash can be looked up or deleted more easily than conventional Multicopy Cuckoo Hashing because of linear probing implemented in the stash.





