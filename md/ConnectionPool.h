#ifndef _MYCONNECTION_POOL_H  
#define _MYCONNECTION_POOL_H  
#include<mysql_connection.h>  
#include<mysql_driver.h>  
#include<cppconn/exception.h>  
#include<cppconn/driver.h>  
#include<cppconn/connection.h>  
#include<cppconn/resultset.h>  
#include<cppconn/prepared_statement.h>  
#include<cppconn/statement.h>  
#include<Poco/Thread.h>  
#include<list>  

using namespace std;  
using namespace sql;  

namespace Pheux { namespace Core {
  class ConnPool{  
    private:  
      int curSize;//��ǰ�ѽ��������ݿ���������  
      int maxSize;//���ӳ��ж����������ݿ�������  
      string username;  
      string password;  
      string url;  
      list<Connection*> connList;//���ӳص���������  
      Poco::Mutex _mutex;;//�߳���  
      static ConnPool *connPool;  
      Driver* driver;  

      Connection*CreateConnection();//����һ������  
      void InitConnection(int iInitialSize);//��ʼ�����ݿ����ӳ�  
      void DestoryConnection(Connection *conn);//�������ݿ����Ӷ���  
      void DestoryConnPool();//�������ݿ����ӳ�  
      ConnPool(string url,string user,string password,int maxSize);//���췽��  
      ConnPool(const string &cfg_filename);//���췽��

    public:  
      ~ConnPool();  
      Connection*GetConnection();//������ݿ�����  
      void ReleaseConnection(Connection *conn);//�����ݿ����ӷŻص����ӳص�������  
      static ConnPool *GetInstance();//��ȡ���ݿ����ӳض���  
  };  
}}
#endif
