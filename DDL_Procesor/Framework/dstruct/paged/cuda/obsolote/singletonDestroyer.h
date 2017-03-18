#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Singleton destroyer. Template class.</summary>
///
/// <remarks>	</remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T> class SingletonDestroyer
{ 
public: 
	SingletonDestroyer(T * = 0); 
	~SingletonDestroyer(); 
	void SetDoomed(T*); 

private: 
	// Prevent users from making copies of a SingletonDestroyer to avoid double deletion: 
	SingletonDestroyer(const SingletonDestroyer &other) {instance = other.instance; } ; 
	//void operator = (const SingletonDestroyer &); 

private: 
	T *instance; 
}; 

template<class T> SingletonDestroyer<T>::SingletonDestroyer(T *_instance)
{ 
	instance =_instance;
} 

template<class T> SingletonDestroyer<T>::~SingletonDestroyer()
{ 
	delete instance;
	instance = 0;
} 

template<class T> void SingletonDestroyer<T>::SetDoomed(T *_instance)
{ 
	instance = _instance; 
} 

