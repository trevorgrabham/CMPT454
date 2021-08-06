When looking for candidates to abort when a deadlock is detected, when considering the number of locks each transaction has, 
we only consider the number of locks that it is the owner of, not including the lock requests that are pending in a lock request
queue for each resource. Because of this, when using the example given in the assignment, we suggest T1 as a candidate for abortion
since it has the same number of locks granted to it as T3, and T3 is the more recent transaction, that being the tie-breaker