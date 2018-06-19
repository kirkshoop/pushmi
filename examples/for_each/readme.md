

## bulk queue insertion

there are multiple ways to achieve this on P1055.

one is to add a type that is an executor but just accumulates a local queue. usage would be similar to..

```cpp
auto pool = thread_pool();

auto e = pool.bulk_executor();
my_bulk_generator(e, . . .);
pool.bulk_enqueue(e);
```

