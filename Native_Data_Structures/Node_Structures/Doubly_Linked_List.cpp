#include<iostream>
using namespace std;

#if __cplusplus < 201103L
	const short nullptr=0L;
#define noexcept
#endif

template<typename T>
class DLL{//i.e. Doubly Linked-List (same as STL::list)
    typedef unsigned long long __uint64;
	typedef unsigned int __uint32;
	
	struct node{
	    T data;
    	node *next,*prev;
    	
    #if __cplusplus < 201103L
    	node(const T& data=T())noexcept://i.e. default ctor
			data(data),next(nullptr),prev(nullptr){}
    #else
		template<typename... _T>
		node(_T&&... data)noexcept://i.e. variadic template ctor
			data{std::forward<_T>(data)...},next(nullptr),prev(nullptr){}
	#endif
		
		friend ostream& operator<<(ostream& out,const node& self){
			out<<self.data;
			return out;
		}
		friend istream& operator>>(istream& in,node& self){
			in>>self.data;
			return in;
		}
	};
	
	struct garbage_collector{//i.e. implementing stack
		static const __uint32 max_capacity=128;
		__uint32 _size;
		node *ptr;
		
		garbage_collector():_size(0),ptr(nullptr){}
		
		void push(node* _new){//i.e. complexity O(1)
			_new->next=ptr;
			ptr=_new;
			++_size;
			if(_size>=max_capacity)
				release();
		}
		void push(node* new_head,node* new_tail,__uint32 new_size){//i.e. complexity O(1)
			new_tail->next=ptr;
			ptr=new_head;
			_size+=new_size;
			if(_size>=max_capacity)
				release();
		}//i.e. useful when a list is destroyed & its' nodes needs to transfer to garbage collector at O(1) complexity
	#if __cplusplus >= 201103L
		template<typename... _T>
		node* pop(_T&&... new_data){//i.e. complexity O(1)
	#else
		node* pop(const T& new_data){
    #endif
			if(!ptr)
    	   		return nullptr;
	    	node *popped=ptr;
    		ptr=ptr->next;
    		--_size;
    	#if __cplusplus >= 201103L
    		popped->data={forward<_T>(new_data)...};
    	#else
    		popped->data=new_data;
    	#endif
    		popped->next=popped->prev=nullptr;
    		return popped;
		}
		void release(){
			node* temp;
			__uint32 prev_size=_size;
			while(_size>prev_size/2){
				temp=ptr->next;
	    		delete ptr;
    			ptr=temp;
				--_size;
			}
		}
		void clear(){//i.e. complexity O(n)
			node* temp;
			while(ptr){
	   			temp=ptr->next;
	    		delete ptr;
    			ptr=temp;	
			}
			_size=0;
		}
		~garbage_collector(){ clear(); }
	};
	
	static garbage_collector GC;
    node *head,*tail;
	__uint32 _size;
public:
	DLL()noexcept:head(nullptr),tail(nullptr),_size(0){}//i.e. default ctor
	
#if __cplusplus >= 201103L
	//i.e.initializer_list based ctor
	DLL(initializer_list<T> list)noexcept:head(nullptr),tail(nullptr),_size(0){
		for(const auto& it:list)//i.e. traversing list through iterator
        	push_back(it);
	}
#endif
	DLL(const DLL &other)noexcept:head(nullptr),tail(nullptr),_size(0){//i.e. copy ctor
    	for(node *it=other.head;it;it=it->next)
           	push_back(it->data);//1) copy other's data
    }
    DLL& operator=(const DLL &other)noexcept{//i.e. copy assignment func.
    	if(this==&other)//i.e. self-assignment protection
    		return *this;
		clear();//1) clear existing resources
        for(node *it=other.head;it;it=it->next)
           	push_back(it->data);//2) copy other's data
        return *this;
    }
#if __cplusplus >= 201103L
 	DLL(DLL&& other)noexcept://i.e. move ctor (C++11 Construct)
  		head(other.head),tail(other.tail),_size(other._size){//1) steal other's data
		other.head=nullptr; other.tail=nullptr; other._size=0;//2) set 'other' to null state
	}//Note: use "-fno-elide-constructors" flag to disable compiler optimization for move ctor (GCC Compilers)
 	
	DLL& operator=(DLL&& other)noexcept{//i.e. move assignment func (C++11 Construct)
  		if(this==&other)
			return *this;
		clear();//1) clear existing resources
  		head=other.head; tail=other.tail; _size=other._size;//2) steal other's data
  		other.head=nullptr; other.tail=nullptr; other._size=0;//3) set 'other' to null state
  		return *this;
	}
#endif

	/* Accessors & Mutators */
#if __cplusplus >= 201103L
	template<typename... _T>
	node* new_node(_T&&... new_data)const{ return GC.ptr?GC.pop(new_data...):new node(new_data...); }
#else
	node* new_node(const T& new_data)const{ return GC.ptr?GC.pop(new_data):new node(new_data); }
#endif
    T& operator[](__uint32 n){
		if (n>=_size){//i.e. parameter is of unsigned type, hence condition 'n<0' is excluded
			cout<<"\nError: Given Index is Out of Bound!\n";
			throw false;
		}
		if(n==_size-1)
			return tail->data;//i.e. same as back()
		node* it=head;
		for(__uint32 i=0;i<n;++i,it=it->next);
		return it->data;
	}
	T& front()const{
		if (head)
			return head->data;
		throw false;
	}
	T& back()const{
		if (tail)
			return tail->data;
		throw false;
	}
	__uint32 size()const{ return _size; }
	
    /*i.e. Modifiers */
#if __cplusplus >= 201103L
	template<typename... _T>
	void push_front(_T&&... new_data){//i.e. emplace_front
		push_front(GC.ptr?GC.pop(new_data...):new node(new_data...));//i.e. direct initialization is possible
#else
    void push_front(const T& new_data){//Complexity: O(1)
		push_front(GC.ptr?GC.pop(new_data):new node(new_data));
#endif
	}
	void push_front(node* new_node){
		new_node->next=head;
		if(head)
			head->prev=new_node;
		else
			tail=new_node;
		head=new_node;
		++_size;
    }
#if __cplusplus >= 201103L
	template<typename... _T>
	void push_back(_T&&... new_data){//i.e. emplace_back
		push_back(GC.ptr?GC.pop(new_data...):new node(new_data...));
#else
    void push_back(const T& new_data){//Complexity: O(1)
    	push_back(GC.ptr?GC.pop(new_data):new node(new_data));
#endif
	}
	void push_back(node *new_node){
		if(!tail)//i.e. anyone of them can be used to check
    		head=tail=new_node;
        else{
			new_node->prev=tail;
			tail->next=new_node;
			tail=new_node;
        }
        ++_size;
	}
    node* pop_front(bool flag=false){//Complexity:O(1)
    	if(!head)
    	    return nullptr;
    	node* popped=nullptr;
    	if (!head->next){
	        if(flag)
				popped=head; 
			else 
				GC.push(head);
	        head=tail=nullptr;
    	}
    	else{
    		node *temp=head->next;
    		if(flag){
    			popped=head;
				popped->next=nullptr;
			}
			else 
				GC.push(head);
    		head=temp;
    		head->prev=nullptr;
    	}
    	--_size;
    	return popped;
	}
	node* pop_back(bool flag=false){//Complexity: O(1)
		if(!tail)
        	return nullptr;
        node* popped=nullptr;
    	if (!tail->prev){
	        if(flag) 
				popped=tail; 
			else 
				GC.push(tail);
	        head=tail=nullptr;
    	}
    	else{
			node *temp=tail->prev;
			if(flag){
				popped=tail;
				popped->prev=nullptr;
			}
			else
				GC.push(tail);
    		tail=temp;
    		tail->next=nullptr;
		}
    	--_size;
    	return popped;
    }
#if __cplusplus >= 201103L
	template<typename... _T>
	void push_middle(_T&&... new_data){//i.e. emplace_middle
		push_middle(GC.ptr?GC.pop(new_data...):new node(new_data...));
#else
	void push_middle(const T& new_data){
	    push_middle(GC.ptr?GC.pop(new_data):new node(new_data));
#endif
	}
	void push_middle(node *new_node){
		if(!head)
			head=tail=new_node;
        else if(!head->next){
			new_node->prev=tail;
			tail->next=new_node;
			tail=new_node;
        }
        else{
    	    node *slow_ptr=head,*fast_ptr=head->next;
    	    while(fast_ptr&&fast_ptr->next){
    	        fast_ptr=fast_ptr->next->next;
    	       	slow_ptr=slow_ptr->next;
    	    }
		    new_node->next=slow_ptr->next;
		    new_node->prev=slow_ptr;
		    slow_ptr->next=new_node;
		    new_node->next->prev=new_node;
		}
	    ++_size;
	}
	node* pop_middle(bool flag=false){
	    if(!head)
        	return nullptr;
        node* popped=nullptr;
    	if (!head->next){
	        if(flag)
				popped=head;
			else
				GC.push(head);
	        head=tail=nullptr;
    	}
    	else{
		    node *slow_ptr=head,*fast_ptr=head;
	        while(fast_ptr&&fast_ptr->next){
    	       	fast_ptr=fast_ptr->next->next;
       	    	slow_ptr=slow_ptr->next;
    		}
    	    slow_ptr->prev->next=slow_ptr->next;
    	    if(slow_ptr->next)
				slow_ptr->next->prev=slow_ptr->prev;
			else
				tail=head;
    		if(flag) 
				popped=slow_ptr;
			else
				GC.push(slow_ptr);
		}
    	--_size;
    	return popped;
	}
#if __cplusplus >= 201103L
	template<typename... _T>
	void insert(__int64 index,_T&&... new_data){//i.e. emplace_randomly
		insert(index,GC.ptr?GC.pop(new_data...):new node(new_data...));
#else
	void insert(__int64 index,const T& new_data){
		insert(index,GC.ptr?GC.pop(new_data):new node(new_data));
#endif
	}
	void insert(__int64 index,node* new_node){
		if(index<0||index>_size)
			return;
    	if(!index){
			push_front(new_node);
			return;
    	}
		if(index==_size){
			push_back(new_node);
			return;
		}
		node *temp=head;
		for(__int64 i=0;i<index-1;++i)
    		temp=temp->next;
    	node* temp2=temp->next;
    	temp->next=new_node;
    	temp2->prev=temp->next;
		temp->next->next=temp2;
		temp->next->prev=temp;
		++_size;
	}
	node* erase(__int64 index,bool flag=false){//i.e. removes node randomly
		if(index<0||index>=_size)
			return nullptr;
    	if(!index)
    		return pop_front(flag);
		if(index==_size-1)
			return pop_back(flag);
		node *temp=head;
		for(__int64 i=0;i<index-1;++i)
    		temp=temp->next;
    	node* temp2=temp->next->next,*popped=nullptr;
    	if(flag)
			popped=temp->next;
		else
			GC.push(temp->next);
    	temp->next=temp2;
    	temp2->prev=temp;
    	--_size;
    	return popped;
	}
private:
	void ascending_sorted_insert(node* new_node){
	    if (!head){
	    	head=tail=new_node;
	    	return;
		}
  		if(new_node->data<head->data){
    	    head->prev=new_node;
    	    new_node->next=head;
    	    head=new_node;
    	    return;
    	}
		if(new_node->data>tail->data){
        	new_node->prev=tail;
        	tail->next=new_node;
        	tail=new_node;
        	return;
    	}
    	node *temp=head->next;
    	while (temp->data<new_node->data)
    		temp=temp->next;
    	temp->prev->next=new_node;
    	new_node->prev=temp->prev;
    	temp->prev=new_node;
    	new_node->next=temp;
	}
	void descending_sorted_insert(node* new_node){
	    if (!head){
	    	head=tail=new_node;
	    	return;
		}
  		if(new_node->data>head->data){
    	    head->prev=new_node;
    	    new_node->next=head;
    	    head=new_node;
    	    return;
    	}
		if(new_node->data<tail->data){
        	new_node->prev=tail;
        	tail->next=new_node;
        	tail=new_node;
        	return;
    	}
    	node *temp=tail->prev;
    	while (temp->data<new_node->data)
    		temp=temp->prev;
    	temp->next->prev=new_node;
    	new_node->next=temp->next;
    	temp->next=new_node;
    	new_node->prev=temp;
	}
public:
#if __cplusplus >= 201103L
	template<typename... _T>
    void sorted_insert(bool direction_flag,_T&&... new_data){
		if(direction_flag)
			ascending_sorted_insert(new node(new_data...));
		else
			descending_sorted_insert(new node(new_data...));
		++_size;
	}
#else
	void sorted_insert(const T& new_data,bool direction_flag=true){
		if(direction_flag)
			ascending_sorted_insert(new node(new_data));
		else
			descending_sorted_insert(new node(new_data));
		++_size;
	}
#endif
	void sort(bool direction_flag=true){//i.e. slightly improved insertion-sort
    	node* current=head;
    	head=tail=nullptr;//i.e. disconnecting links to 'current' list
    	while (current){
        	node* temp=current->next;
        	current->prev=current->next=nullptr;//i.e. disconnecting links to 'current' list
        	if(direction_flag)
				ascending_sorted_insert(current);
			else
				descending_sorted_insert(current);
        	current=temp;
    	}
	}
	void reverse(){
		if(!head||!head->next)
			return;
		node* temp=head;
		head=tail;
		tail=temp;
	#if __cplusplus >= 201103L	
		for(auto& it:*this){
    		temp=it.next;
    		it.next=it.prev;
    		it.prev=temp;
		}
	#else
		for(node *it=head;it;it=it->next){
    		temp=it->next;
    		it->next=it->prev;
    		it->prev=temp;
		}
	#endif
	}
    void traverse_forward()const{
    	if(!head){
    		cout<<"List is empty!"<<endl;
        	return;
    	}
	#if __cplusplus >= 201103L
		for(const auto& it:*this)
			cout<<it<<" ";
	#else
		for(node *it=head;it;it=it->next)
    		cout<<*it<<" ";
	#endif
		cout<<endl;
	}
	void traverse_backward()const{
    	if(!tail){
       		cout<<"List is empty!"<<endl;
        	return;
    	}
    #if __cplusplus >= 201103L
    	for(const auto& it:reverse_adapter(*this))
			cout<<it<<" ";
	#else
		for(node *it=tail;it;it=it->prev)
    		cout<<*it<<" ";
	#endif
		cout<<endl;
	}
	void clear(){
		if(head){
			GC.push(head,tail,_size);
			tail=head=nullptr;
		}
		_size=0;
	}
	
	/* Operations (two list based)*/
	void swap(DLL &other){
	#if __cplusplus >= 201103L
    	DLL temp(move(*this));//i.e. here, move ctor gets invoked for pointed obj
		*this=move(other);
	    other=move(temp);
	#else
    	DLL temp(*this);
    	*this=other;
    	other=temp;
	#endif
	}
	void merge(DLL &other,__int64 pos=0){
		if(!other.head||pos<0||pos>_size)
			return;//i.e. do nothing if 'other' have no nodes or size exceeds the given pos
		node *temp=nullptr;
		if(!pos){//i.e. if pos is '0', then directly join head of other to head of 'this'
			other.tail->next=head;
			head->prev=other.tail;
			head=other.head;
		}
		else if(pos==_size){
			tail->next=other.head;
			other.head->prev=tail;
			tail=other.tail;
		}
		else{
			node *temp2=head;
			for(int i=1;i<pos;++i)
				temp2=temp2->next;
			temp=temp2->next;
			temp2->next=other.head;
			other.head->prev=temp2;
			other.tail->next=temp;
			temp->prev=other.tail;
		}
		_size+=other._size;
		other.head=other.tail=nullptr;
		other._size=0;
	}

 	/* Iterators */
	template<bool flag>//i.e. true-> forward_iterator, false->reverse_iterator
	struct _iterator{
		typedef _iterator self;
		
		_iterator():ptr(nullptr){}
    	_iterator(node* ptr):ptr(ptr){}
    	
    	node& operator*()const{ return *ptr; }
    	node* operator->()const{ return ptr; }
    	
    	self& operator++(){ flag?ptr=ptr->next:ptr=ptr->prev; return *this; }
    	self& operator--(){ flag?ptr=ptr->prev:ptr=ptr->next; return *this; }
		self operator++(int){ self temp(*this); flag?ptr=ptr->next:ptr=ptr->prev; return temp; }
		self operator--(int){ self temp(*this); flag?ptr=ptr->prev:ptr=ptr->next; return temp; }
		
		bool operator==(const self& other)const{ return ptr==other.ptr; }
		bool operator!=(const self& other)const{ return ptr!=other.ptr; }
	private:
		node* ptr;
    };
    typedef _iterator<true> iterator;
    typedef _iterator<false> r_iterator;
	
    iterator begin()const{ return iterator(head); }
	iterator end()const{ return iterator(nullptr); }	
    r_iterator rbegin()const{ return r_iterator(tail); }
	r_iterator rend()const{ return r_iterator(nullptr); }
#if __cplusplus >= 201103L
private:
	struct reverse_adapter{//i.e. defined to use reverse_iterator on 'range-based' loop
		reverse_adapter(const DLL& ref):ref(ref){}
		r_iterator begin(){ return ref.rbegin(); }
		r_iterator end(){ return ref.rend(); }
	private:
		const DLL& ref;
	};
public:
	//i.e. reversely traverse DLL without changing it (inside range-based loop)
	reverse_adapter reverse_iterator()const{ return reverse_adapter(*this); }
#endif
	
	/* Garbage Collector Operations */
	static void clear_gc(){ GC.clear(); }//i.e. destroys all the preserved nodes in garbage_collector of 'T' type
	static void release_gc(){ GC.release(); }//i.e. destroys half of the total preserved nodes....
	
	~DLL(){ clear(); }//i.e. dtor
};
template<typename T>
typename DLL<T>::garbage_collector DLL<T>::GC;


void test_case_1(){
	#if __cplusplus >= 201103L
	DLL<int> obj{1,2,3,4,5,6,7,8,9},obj2{99,88,77,66,55};
#else
	DLL<int> obj,obj2;
	for(int i=1;i<=9;++i)
		obj.push_back(i);
	for(int i=99;i>=55;i-=11)
		obj2.push_back(i);
#endif
	obj.traverse_forward();
	//	for(DLL<int>::iterator<false> it=obj.begin();it!=obj.end();++it)
	//		cout<<*it<<", ";
	
	obj.traverse_backward();
	//	for(DLL<int>::iterator<true> it=obj.rbegin();it!=obj.rend();++it)
	//		cout<<*it<<", ";
	
	obj.merge(obj2);
	
//	obj.sort();
	
//	obj.sorted_insert(1,404);//i.e. works on sorted data
	
//	obj.erase(5);

//	obj.insert(4,404);

//	obj.push_middle(404);

//	obj.pop_middle();
	
//	for(int i=1;i<=5;++i)
//		obj.push_front(i);
		
//	for(int i=0;i<5;++i)
//		obj.pop_front();
	
//	for(int i=0;i<5;++i)
//		obj.pop_back();
	
	obj.traverse_forward();
	obj.traverse_backward();
}

#if __cplusplus >= 201103L
void test_case_2(){
	cout<<"test_case_2:"<<endl;
	DLL<DLL<int>> obj{{1,2,3,4,5},{6,7,8,9,10}},obj2;
	
	obj.push_back(11,12,13,14,15);
	obj.push_front(-5,-4,-3,-2,-1);
	
	cout<<"---------------"<<endl;
	for(const auto& _it:obj){
		for(const auto& it:_it.data)
			cout<<it<<" ";
		cout<<endl;
	}
	cout<<"---------------"<<endl;
	for(const auto& _it:obj.reverse_iterator()){
		for(const auto& it:_it.data.reverse_iterator())
			cout<<it<<" ";
		cout<<endl;
	}
//	obj2=obj; 
	obj2=move(obj);//i.e. same as swap
//	obj.swap(obj2);

	cout<<"---------------"<<endl;
	for(const auto& it:obj2)
		it.data.traverse_forward();
	cout<<"---------------"<<endl;
	for(const auto& it:obj2.reverse_iterator())
		it.data.traverse_backward();
	cout<<"---------------"<<endl;
}
#endif

void test_case_3(){//i.e. C.R.U.D Operations on Queue
	DLL<int> list;
	short choice;
	while (true){
		cout<<"\nSELECT YOUR CHOICE:-\n"
			<<"\n1. Insert value (at back)"
			<<"\n2. Update Value (random)"
			<<"\n3. Delete value (at front)"
			<<"\n4. View List"
			<<"\n5. Exit\n\n";
		cin>>choice;
		fflush(stdin);
		cin.clear();
		system("cls");
		switch (choice){
		case 1:{
			int val;
			cout<<"Enter value: ";
			cin>>val;
			list.push_back(val);
			break;
		}
		case 2:{
			int val,n;
			cout<<"Enter index: ";
			cin>>n;
			if(n<list.size()){
				cout<<"Enter value: ";
				cin>>val;
				list[n]=val;
			}
			break;
		}
		case 3:
			list.pop_front();
			break;
		case 4:
			cout<<"List: ";
			list.traverse_forward();
			break;
		case 5:
			return;
		}
	}
}


int main(){
//	test_case_1();//use of all operations
//	test_case_2();//nested 2D DLL testing (C++11)
	test_case_3();//using DLL as queue (best-case)
	return 0;
}
