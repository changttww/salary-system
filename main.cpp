#include <iostream>
#include <string>
#include <vector>
#include <map>
#include<fstream>
#include<sstream>
#include <functional>
#include <iomanip>
using namespace std;

class Command;
//写一个person抽象基类，然后派生出manager和saleman，然后这俩那派生出salemanager
class Person {
protected:
    int employeeID =0 ;       // 职工编号
    string name;     // 姓名
    int level;            // 等级
    double salary;        // 薪水
    static int count;      // 静态数据成员，记录当前员工数量

public:
    friend Command;
    // 构造函数
    Person(int id, const string& name, int position, double salary)
        :  employeeID(id),name(name), salary(salary) {
            setLevel(position);    // 根据职位设置等级
        }
    
    // 设置等级函数
    void setLevel(int position) {
        level = position;
    }

    string getName() const {
        return name;
    }
    int getid()
    {
        return employeeID;
    }
    int getLevel() const {
        return level;
    }

    void displaysalary(){
     cout << setw(10) << employeeID << setw(10)<< name << setw(10)<< level  << setw(10)<< salary << endl;
    }

    // 纯虚函数，抽象类接口
    virtual void displayInfo() const = 0; 
    virtual double getSalary() const = 0;
    virtual void pay() = 0;

    // 虚析构函数，确保派生类可以正确释放资源
    virtual ~Person() = default;
};
int Person::count =0;

class Manager : virtual public Person{
    public:
    //构造函数
    Manager(int id,const string& name, double salary) : Person(id, name, 4, salary) {}

    // 重写父类虚函数
    double getSalary() const override {
        return 18000 ; 
    }
    
    void pay() override {
        salary = getSalary();
        cout << "paying manager " << getName() << " 固定薪资 " <<  getSalary() << " permonth" << endl;
    }
    void displayInfo() const override {
        cout << "EmployeeID: " << employeeID << ", Name: " << name << ", Level: " << level << ", Salary: " << salary << endl;
    }
};

class Saleman : virtual public Person{
    protected:
    double monthlysales;  // 月度销售额

    public:
    friend Command;
    //构造函数
    Saleman(int id, const string& name, double salary, double monthlysales) : Person(id,name, 1, salary) ,monthlysales(monthlysales)
    {
        // cout << "constructor:"<<monthlysales <<endl;
    }

    // 重写父类虚函数
    double getSalary() const override {
        return monthlysales*0.04; 
    }
    void pay() override {
        double sales;
        cout << "请输入销售员 " << getid() << " "<<  getName() << " 本月销售额" << endl;
        cin >> sales;
        monthlysales = sales;
        salary = getSalary();
        cout << "paying saleman " << getName() << " " << getSalary() <<" this month" << endl;
    }
    void displayInfo() const override {
        cout << "EmployeeID: " << employeeID << ", Name: " << name << ", Level: " << level << ", Salary: " << salary << ", Monthly Sales: " << monthlysales <<endl;
    }
};

class SaleManager : public Manager, public Saleman{
    public:
    friend Command;
    //构造函数
    SaleManager(int id, const string& name, double salary, double monthlysales) : Person(id,name, 2, salary), Manager(id, name, salary), Saleman(id, name, salary, monthlysales){}

    // 重写父类虚函数
    double getSalary() const override {
        return 5000 + monthlysales * 0.02;
    }

    void pay() override {
        double sales;
        cout << "请输入销售经理 " << getid() << " "<<  getName() << " 本月销售总额" << endl;
        cin >> sales;
        monthlysales = sales;
        salary = getSalary();
        cout << "paying salemanager " << getName() << " " << getSalary() <<" this month" << endl;
    }
    void displayInfo() const override {
        cout << "EmployeeID: " << employeeID << ", Name: " << name << ", Level: " << level << ", Salary: " << salary << ", Total Monthly Sales: " << monthlysales <<endl;
    }
};

class Technician : public Person{
    protected:
    double monthlyHours;  // 月度工作时长
    public:
    friend Command;
    //构造函数
    Technician(int id, const string& name, double salary, double monthlyhours) : Person(id, name, 3, salary), monthlyHours(monthlyhours){}
    // 重写父类虚函数
    double getSalary() const override {
        return monthlyHours * 100;
    }
    void pay() override {
        double worktime ;
        cout << "请输入技术人员 " << getid() << " "<<  getName() << " 本月工作时长" << endl;
        cin >> worktime;
        monthlyHours = worktime;
        salary = getSalary();
        cout << "paying technician " << getName() << " " << getSalary() << " this month" << endl;
    }
    void displayInfo() const override {
        cout << "EmployeeID: " << employeeID << ", Name: " << name << ", Level: " << level << ", Salary: " << salary << ", Work Time: " << monthlyHours <<endl;
    }
};

//写一个command类，监听命令行传入的命令

class Command {
     //定义一个哈希表，用员工id作为key,将对应对象的person类指针作为value
    map<int, Person*> m_person;
    map<string, function<void()>> commandMap;

    void registerCommand(const std::string& command, std::function<void()> func) {
        commandMap[command] = func;
    }
    
    
    // 依照职位创建并添加到map中
    void addmap(int level,int id, string& name,double salary,double sales,double workTime){
         if (level == 1) {
            // cout << sales <<endl;
            auto ptr = new Saleman(id,name, salary, sales);
            // cout <<ptr->monthlysales <<endl;
            m_person.insert({id, ptr});
        } else if (level == 2) {
            m_person.insert({id, new SaleManager(id,name, salary, sales)});
        } else if (level == 3) {
             m_person.insert({id, new Technician(id,name, salary, workTime)});
        } else {
            auto ptr= new Manager(id,name, salary);
            //cout << ptr->getid() << " " << ptr->getName() << endl;
            m_person.insert({id, ptr});
        }
    }
    //读取csv文件
void parseCSV(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    
    // 跳过CSV文件的标题行
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string item;
        

        // 解析每一行的字段
        std::getline(ss, item, ',');
        int id= std::stoi(item);

        std::getline(ss, item, ',');
        string name = item;
    
        std::getline(ss, item, ',');
        int level = std::stoi(item);

        std::getline(ss, item, ',');
        int salary = std::stod(item);

        std::getline(ss, item, ',');
        double sales = std::stod(item);


        std::getline(ss, item, ',');
        double workTime = std::stod(item);

        addmap(level,id,name,salary,sales,workTime);
        
        Person:: count = max(id,Person::count);
        // cout << "解析1次\n";
    }

    

}

//添加员工
void createemployees(){
    int id ;
    int level;
    string input;
    string name;
    Person :: count = 2000;

    cout << "录入职工资料会覆盖原先数据，请慎重！！！" << endl;
    cout << "继续该操作吗？(Y/y):" << endl;
    cin >> input;
    if(input!= "Y" && input!= "y"){
        cout << "已取消操作" << endl;
        return ;
    }
    
    //清空原有哈希表
    for(auto it = m_person.begin(); it != m_person.end();++it){
        delete it->second;
    }
    m_person.clear();

    cout << "输入职工花名册"<< endl;

    cout << "--------------------------------" << endl;
    cout << "输入经理姓名" << endl;
    cin >> name ;
    id = ++Person::count;
    addmap(4,id,name,0,0,0);

    cout << "输入技术人员姓名，若输入@表示结束" << endl;
    while(true){
        cout << "请输入员工姓名：" << endl;
        cin >> name ;
        if(name == "@") break;
        id = ++Person::count;
        addmap(3,id,name,0,0,0);
    }
     cout << "输入销售经理姓名，若输入@表示结束" << endl;
    while(true){
        cout << "请输入员工姓名：" << endl;
        cin >> name ;
        if(name == "@") break;
        id = ++Person::count;
        addmap(2,id,name,0,0,0);

    }

     cout << "输入销售人员姓名，若输入@表示结束" << endl;
    while(true){
        cout << "请输入员工姓名：" << endl;
        cin >> name ;
        if(name == "@") break;
        id = ++Person::count;
        addmap(1,id,name,0,0,0);

    }

}


void addemployee(){
    int id;
    int level;
    string name;
    cout << "请输入员工姓名：" << endl;
    cin >> name ;
    cout << "请输入职位等级" << endl;
    cout << "1.销售员 " << endl;
    cout << "2.销售经理 " << endl;
    cout << "3.技术人员 " << endl;
    cout << "4.经理" << endl;
    cin >> level;

    id = ++Person::count;
    addmap(level,id,name,0,0,0);

    cout << "已添加员工 " << id << " " <<  name << endl;
}

void removemployee(){
    int id;
    string name;
    cout << "请输入员工编号：" << endl;
    cin >> id ;
    //检查员工是否存在
    if(m_person.find(id) == m_person.end()){
        cout << "该员工不存在" << endl;
        return;
    }
    else {
        name = m_person[id]->name;
        Person* person = m_person[id];
        delete person;
        m_person.erase(id);
        cout << "编号为 " << id << " 姓名为 " << name << " 的员工信息已删除" << endl;
    }

}

void updateployee(){
    int id;
    int level;
    cout << "请输入员工编号：" << endl;
    cin >> id ;
    //检查员工是否存在
    if(m_person.find(id) == m_person.end()){
        cout << "该员工不存在" << endl;
        return;
    }
    else {
        string name = m_person[id]->name;
        cout << "请输入员工 " << id << " "<<  name << " 新职位等级" << endl;
        cout << "1.销售员 " << endl;
        cout << "2.销售经理 " << endl;
        cout << "3.技术人员 " << endl;
        cout << "4.经理" << endl;
        cin >> level;
        //确认输入是否为1-5的整数
        if(level < 1 || level > 4){
            cout << "无效的操作" << endl;
            return;
        }
        else {
            delete m_person[id];
            m_person.erase(id);
            addmap(level,id,name,0,0,0);
        }
        }
}

// 将修改后的员工数据写回CSV文件
void writeCSV(const std::string& filename) {
    std::ofstream file(filename,std::ios::out | std::ios::trunc);
    
    // 写入CSV的标题行
    file << "员工编号,姓名,级别,薪水,销售额,工作时间\n";
    
    // 写入每个员工的数据
    for (auto it = m_person.begin(); it != m_person.end(); ++it) {
        file << it->first<< ","
             << it->second->name<< "," 
             << it->second->level<< "," 
             << it->second->salary << "," ;
             if(it->second->level == 1 || it->second->level == 2)
                file<< dynamic_cast<Saleman*>(it->second)->monthlysales << ",";
            else file<< 0 << "," ;
             
             if(it->second->level == 3 )
             file << dynamic_cast<Technician*>(it->second)->monthlyHours << ","  << "\n";
             else  file<< 0 << "," <<"\n";
    }
}

    //通过员工id找到对应的基类指针
    Person* find_person(const int id){
        if(m_person.find(id) == m_person.end()){
            cout << "id is not found" << endl;
            return nullptr;  
        }
        else return m_person[id];
    }
    
    void paySalary(Person* employee) {
        employee->pay();
    }

    void payAll(){
        for(auto it = m_person.begin(); it != m_person.end(); ++it){
            it->second->pay();
        }
    }

    void displayEmployeeInfo() {
        
        for( auto it = m_person.begin(); it != m_person.end(); ++it ) {
        it->second->displayInfo();
       }
    }

    void displayEmployeesalary(){
        int totalSalary = 0;
        cout << "              员 工 薪 酬 表" << endl;
        cout << "---------------------------------------------\n";
        cout << setw(17) << "职工编号" << setw(10) << "姓名" << setw(12) << "级别" << setw(12) << "月工资" << endl;
        cout << "---------------------------------------------\n";

        for( auto it = m_person.begin(); it!= m_person.end(); ++it){
            it->second->displaysalary();
            totalSalary += it->second->salary;
        cout << "---------------------------------------------\n";
        }
        cout  << setw(32) << "合计  " <<  setw(10) << totalSalary << "\n";
    }
    
    void setMonthlySales(Person* employee, double monthlySales) {
        if(employee->level == 2 || employee->level == 1)
        dynamic_cast<Saleman*>(employee)->monthlysales = monthlySales;
        else cout << "找不到销售额" << endl;
    }
    
    void setMonthlyHours(Person* employee, double monthlyHours) {
        if(employee->level == 3)
        dynamic_cast<Technician*>(employee)->monthlyHours = monthlyHours;
        else cout << "找不到工作时间" << endl;
    }
    
void showHelp() {
    // ANSI转义码颜色
    const std::string reset = "\033[0m";
    const std::string bold = "\033[1m";
    const std::string cyan = "\033[36m";
    const std::string green = "\033[32m";
    const std::string yellow = "\033[33m";
    const std::string red = "\033[31m";

    // 标题部分
    std::cout << cyan << std::endl << bold << "✨✨✨----------- 公司薪酬管理系统 ------------✨✨✨" << reset << std::endl;

    // 菜单选项
    std::cout << yellow << "可用命令选项：" << reset << std::endl;
    std::cout << green << " 1️⃣ 录入职工资料➡️" << reset << std::endl;
    std::cout << green << " 2️⃣ 增加职工➡️" << reset << std::endl;
    std::cout << green << " 3️⃣ 删除职工🛠️" << reset << std::endl;
    std::cout << green << " 4️⃣ 修改职工资料🛠️" << reset << std::endl;
    std::cout << green << " 5️⃣ 月薪发放📢" << reset << std::endl;
    std::cout << green << " 6️⃣ 显示工资表💡" << reset << std::endl;
    std::cout << green << " 7️⃣ 显示员工资料💡" << reset << std::endl;
    std::cout << red << " 0️⃣ 退出系统🔙" << reset << std::endl;

    // 结束线
    std::cout << cyan << bold << "---------------------------------------------------------" << reset << std::endl;
}

    
   
    
public:
    Command() {
        // cout << "开始解析\n";
        parseCSV("person.csv");
        // cout << "停止解析\n";
        registerCommand("0", []() {
            std::cout << "退出命令注册，仅用于说明，不实际执行退出操作。" << std::endl;
        });
        registerCommand("1", [this]() {
            createemployees();
        });
        registerCommand("2", [this]() {
            addemployee();
        });
        registerCommand("3", [this]() {
            removemployee();
        });
        registerCommand("pay", [this]() {
            std::cout << "请输入需要发放工资的员工 ID: ";
            int id;
            std::cin >> id;
            auto it = find_person(id);
            paySalary(it);
        });
        registerCommand("4", [this]() {
            updateployee();
        });
        registerCommand("5", [this]() {
            payAll();
        });
        registerCommand("6", [this]() {
            displayEmployeesalary();
        });
        registerCommand("7", [this]() {
            displayEmployeeInfo();
        });
    
    }
    void run() {
        std::string input;
        while (true) {
            showHelp();
            cout << "❓请输入命令>>";
            // std::getline(std::cin, input);
            cin >> input;
            // 输入 "exit" 时退出循环
            if (input == "0") {
                std::cout << "退出监听。" << std::endl;
                break;
            }

            // 查找并执行命令
            auto it = commandMap.find(input);
            if (it != commandMap.end()) {
                it->second(); // 执行对应的函数
            } else {
                std::cout << "未知命令: " << input << std::endl;
            }
            // 清除输入缓冲区中的换行符
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "按回车键继续..." << std::endl;
                std::cin.get();  
        }
    }   
    ~Command(){
        writeCSV("person.csv");
    };
   
};

int main()
{
    Command command;
    // cout << "test\n";
    command.run();
    // cout << "test\n";
    return 0;
}
