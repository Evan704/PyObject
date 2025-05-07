# PyObject
PyObject可以存储**任意类型**的数据，并且在运行时**动态**改变数据的类型。
对于**基本类型**，PyObject会复制原值。
对于**复杂类型**，如果赋值方式是：
1. **左值**：PyObject不获得所有权，仅保存引用。
2. **右值**：PyObject获得所有权，对其负责。
3. **PyObject**：如果对方拥有所有权，所有权转移，旧PyObject丧失所有权，新PyObject获得所有权；如果对方是引用，则新PyObject获得对原对象的引用。
4. __PyObject *__: 不获得所有权，保存指向原对象的引用。

对于**PyObject**向**PyObject以外的类**的对象赋值：
1. **T**：复制，PyObject的所有权不改变。
2. **T &**：返回一份引用，PyObject的所有权不改变。