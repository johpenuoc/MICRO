cls Something :: InheritedClass
{
    @ _init
    {
        this.obj1 = "hello";
        this.obj2 = 10;
    };
    
    fn change_obj1(change_to)
        {
            this.obj1 = change_to;
        };
};

a = Something;
a.obj1 = "hi";