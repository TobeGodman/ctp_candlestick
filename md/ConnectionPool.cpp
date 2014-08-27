#include<stdexcept>  
#include<exception>  
#include<stdio.h> 
#include "Config.h"
#include"ConnectionPool.h"  


using namespace std;  
using namespace sql;  
namespace Pheux { namespace Core {
  ConnPool*ConnPool::connPool=NULL;  
  //���ӳصĹ��캯��
  ConnPool::ConnPool(const string& cfg_filename)  
  {  
    Config::Instance()->parse_xml();
    this->maxSize=Config::Instance()->Getmaxsize();
    this->curSize=0;  
    this->username=Config::Instance()->Getusername();
    this->password=Config::Instance()->Getmysql_password();  
    this->url=Config::Instance()->Getmysql_tcp() + ":" + Config::Instance()->Getmysql_port();
    try{  
      this->driver=sql::mysql::get_driver_instance();  
    }  
    catch(sql::SQLException &e)  
    {  
      perror("�������ӳ���;\n");  
    }  
    catch(std::runtime_error &e)  
    {  
      perror("���г�����\n");  
    }  
    this->InitConnection(maxSize/2);  
  }  
  ConnPool::ConnPool(string url, string userName,string password, int maxSize)  
  {  
    this->maxSize=maxSize;  
    this->curSize=0;  
    this->username=userName;  
    this->password=password;  
    this->url=url;  
    try{  
      this->driver=sql::mysql::get_driver_instance();  
    }  
    catch(sql::SQLException &e)  
    {  
      perror("�������ӳ���;\n");  
    }  
    catch(std::runtime_error &e)  
    {  
      perror("���г�����\n");  
    }  
    this->InitConnection(maxSize/2);  
  }  
  //��ȡ���ӳض��󣬵���ģʽ  
  ConnPool*ConnPool::GetInstance(){  
    if(connPool==NULL)  
    {  
      connPool=new ConnPool("config.xml");  
    }  
    return connPool;  
  }  
  //��ʼ�����ӳأ����������������һ����������  
  void ConnPool::InitConnection(int iInitialSize)  
  {  
    Connection*conn;  
    _mutex.lock();  
    for(int i=0;i<iInitialSize;i++)  
    {  
      conn=this->CreateConnection();  
      if(conn){  
        connList.push_back(conn);  
        ++(this->curSize);  
      }  
      else  
      {  
        perror("����CONNECTION����");  
      }  
    }  
    _mutex.unlock();  
  }  
  //��������,����һ��Connection  
  Connection*ConnPool::CreateConnection(){  
    Connection*conn;  
    try{
		cout << this->url << endl;
	    cout << this->username << endl;
		cout << this->password <<endl;
      conn=driver->connect(this->url.c_str(),this->username.c_str(),this->password.c_str());//�������� 
	  cout << "one more is done��" <<endl;
      return conn;  
    }  
    catch(sql::SQLException &e)  
    {  
      cout << "error1" <<endl;  
      return NULL;  
    }  
    catch(std::runtime_error &e)  
    {  
      cout << "error2" <<endl;  
      return NULL;  
    }  
  }  
  //�����ӳ��л��һ������  
  Connection*ConnPool::GetConnection(){  
    Connection*con;  
    _mutex.lock();  
    if(connList.size()>0)//���ӳ������л�������  
    {  
      con=connList.front();//�õ���һ������  
      connList.pop_front();//�Ƴ���һ������  
      if(con->isClosed())//��������Ѿ����رգ�ɾ�������½���һ��  
      {  
        delete con;  
        con=this->CreateConnection();  
      }  
      //�������Ϊ�գ��򴴽����ӳ���  
      if(con==NULL)  
      {  
        --curSize;  
      }  
      _mutex.unlock();  
      return con;  
    }  
    else{  
      if(curSize< maxSize){//�����Դ����µ�����  
        con= this->CreateConnection();  
        if(con){  
          ++curSize;  
          _mutex.unlock();  
          return con;  
        }  
        else{  
          _mutex.unlock();  
          return NULL;  
        }  
      }  
      else{//�������������Ѿ��ﵽmaxSize  
        _mutex.unlock();  
        return NULL;  
      }  
    }  
  }  
  //�������ݿ�����  
  void ConnPool::ReleaseConnection(sql::Connection * conn){  
    if(conn){  
      _mutex.lock();  
      connList.push_back(conn);  
      _mutex.unlock();  
    }  
  }  
  //���ӳص���������  
  ConnPool::~ConnPool()  
  {  
    this->DestoryConnPool();  
  }  
  //�������ӳ�,����Ҫ���������ӳص�������  
  void ConnPool::DestoryConnPool(){  
    list<Connection*>::iterator icon;  
    _mutex.lock();  
    for(icon=connList.begin();icon!=connList.end();++icon)  
    {  
      this->DestoryConnection(*icon);//�������ӳ��е�����  
    }  
    curSize=0;  
    connList.clear();//������ӳ��е�����  
    _mutex.unlock();  
  }  
  //����һ������  
  void ConnPool::DestoryConnection(Connection* conn)  
  {  
    if(conn)  
    {  
      try{  
        conn->close();  
      }  
      catch(sql::SQLException&e)  
      {  
        perror(e.what());  
      }  
      catch(std::exception&e)  
      {  
        perror(e.what());  
      }  
      delete conn;  
    }  
  } 

}}
