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

a := Something;

fn ohyeah(a)
{
    i = 0;
    until (i == 10)
        {
            a.pigeon += "N";
            
            i += 1;
        };
};

ohyeah(a);
