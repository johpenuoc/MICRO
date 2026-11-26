cls InheritedClass
{
    @ _init
    {
        this.pigeon = "PIJOHN";
    };
    
    fn ic_function()
        {
            this.pigeon = "PIJOHNS RULE";
        };
};

cls Something :: InheritedClass
{
    @ _init
    {
        this.obj1 = "hello";
        this.obj2 = 10;
    };
    
    fn something_func()
        {
            b = 6;
            
            this.ic_function();
            
            a = 5;
        };
};

a = Something;
a.obj1 = "hi";