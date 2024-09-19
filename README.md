Реализован STL-совместимый контейнер для [BinarySearchTree](https://en.wikipedia.org/wiki/Binary_search_tree), реализующий различные [способы обхода дерева (in-, pre-, post-order)](https://en.wikipedia.org/wiki/Tree_traversal) через итераторы. Сделано это с помощью идиомы [Tag Dispatch](https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Tag_Dispatching)

Контейнер представляет из себя шаблон, праметрезируемый типом хранимых объектов, оператором сравнения и аллокатором, а так же удовлетворяет следующим требованиям к stl - совместимым контейнерам:

  - [контейнер](https://en.cppreference.com/w/cpp/named_req/Container)
  - [ассоциативный контейнер](https://en.cppreference.com/w/cpp/named_req/AssociativeContainer)
  - [контейнер с обратным итератором](https://en.cppreference.com/w/cpp/named_req/ReversibleContainer)
  - [контейнер с двунаправленным итератором](https://en.cppreference.com/w/cpp/named_req/BidirectionalIterator)
  - [контейнер, поддерживающий аллокатор](https://en.cppreference.com/w/cpp/named_req/AllocatorAwareContainer)
