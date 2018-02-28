/*
 *    This file is part of RCBot.
 *
 *    RCBot by Paul Murphy adapted from botman's template 3.
 *
 *    RCBot is free software; you can redistribute it and/or modify it
 *    under the terms of the GNU General Public License as published by the
 *    Free Software Foundation; either version 2 of the License, or (at
 *    your option) any later version.
 *
 *    RCBot is distributed in the hope that it will be useful, but
 *    WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with RCBot; if not, write to the Free Software Foundation,
 *    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    In addition, as a special exception, the author gives permission to
 *    link the code of this program with the Half-Life Game Engine ("HL
 *    Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *    L.L.C ("Valve").  You must obey the GNU General Public License in all
 *    respects for all of the code used other than the HL Engine and MODs
 *    from Valve.  If you modify this file, you may extend this exception
 *    to your version of the file, but you are not obligated to do so.  If
 *    you do not wish to do so, delete this exception statement from your
 *    version.
 *
 */
/****************************************************
 * RCBOT Generic Classes                            *
 *                                                  *
 * Code by : Paul Murphy {paul.murphy@ntlworld.com} *
 *                       {cheeseh@rcbot.net}        *
 *                                                  *
 ****************************************************
 *
 * Copyright (c) 2003 Paul M'(cheeseh@rcbot.net/paul.murphy@ntlworld.com)
 * If using any of these components please keep this tag
 *
 ****************************************************
*/

#ifndef __RCBOT_GENERIC_CLASS__
#define __RCBOT_GENERIC_CLASS__

#include <assert.h>
#include <vector>
using namespace std;

void DebugMessage ( int iDebugLevel, edict_t *pEntity, int errorlevel, char *fmt, ... );
void BotMessage (edict_t *pEntity, int errorlevel, char *fmt, ... );
void BugMessage (edict_t *pEntity, char *fmt, ...);
//////////////////////////////////////////////////////////////////////
// GENERIC CLASSES

// Typical Node
template <class T>
class dataNode
{
	public:
		dataNode()// constructor
		{
			m_Next = NULL;
		}

		void _delete ()
		{
			m_Next = NULL;
		}
		/*
		~dataNode()
		{			
			m_Next = NULL;
		}
		*/

		T m_NodeData;
		dataNode<T> *m_Next;
};

template <class T>
class dataQueue
{
	public:
		dataQueue()// constructor -- Must be public.
		{
			this->Init();
		}

		/*~dataQueue()
		{
			this->Destroy();
		}*/

		// explicit delete function only now..
		void _delete ()
		{
			this->Destroy();
		}

		inline void Init ( void )
		{
			m_Head = NULL;
			m_Tail = NULL;
		}

		// safe get tail op
		/*dataNode<T> *Tail ( void )
		{
			dataNode<T> *tempNode = m_Head;

			while ( tempNode && tempNode->m_Next )
			{
				tempNode = tempNode->m_Next;
			}

			return tempNode;		
		}*/

		void Destroy ( void )
		{
			dataNode<T> *tempNode = NULL;

			while ( m_Head )
			{
				tempNode = m_Head;

				m_Head = m_Head->m_Next;

				tempNode->_delete();
				delete tempNode;

				tempNode = NULL;
			}

			m_Head = NULL;
			m_Tail = NULL;
		}

		inline BOOL IsEmpty ( void )
		{
			return (( m_Head == NULL )||(m_Tail == NULL));
		}

		void AddFront ( const T &pObj )
		{
			dataNode<T> *newNode = new dataNode<T>;					

			newNode->m_NodeData = pObj;

			if ( m_Head == NULL )
			{
				m_Tail = newNode;
				m_Head = newNode;
			}
			else
			{				
				newNode->m_Next = m_Head;
				m_Head = newNode;
			}
		}

		void Add ( const T &pObj )
		{
			dataNode<T> *newNode = new dataNode<T>;
			newNode->m_NodeData = pObj;

			if ( IsEmpty() )
			{
				//newNode->m_Next = m_Head;

				m_Head = newNode;
				m_Tail = newNode;
			}
			else
			{
				m_Tail->m_Next = newNode;
				m_Tail = newNode;
			}
		}

		inline T GetFrontInfo ( void )
		{
			return m_Head->m_NodeData;
		}

		inline T *GetFrontPointer ( void )
		{
			return &m_Head->m_NodeData;
		}

		T ChooseFrom ( void )
		{
			T *l_pTemp;

			try
			{
				l_pTemp = &m_Head->m_NodeData;

				m_Head = m_Head->m_Next;
			}

			catch ( ... )
			{
				// problem
				BugMessage(NULL,"Bad pointer in queue, (Resetting queue) Memory may have been lost");

				m_Head = NULL;
				m_Tail = NULL;
				// return default
				return T();
			}

			return *l_pTemp;
		}

		T *ChoosePointerFrom ( void )
		{
			T *l_pTemp;

			try
			{
				l_pTemp = &m_Head->m_NodeData;
				
				m_Head = m_Head->m_Next;
			}

			catch ( ... )
			{
				// problem
				BugMessage(NULL,"Bad pointer in queue, (Resetting tasks) Memory may have been lost");

				m_Head = NULL;
				m_Tail = NULL;
				// return default
				return NULL;
			}

			return l_pTemp;
		}

		void RemoveFront ( void )
		{
			dataNode<T> *tempNode = m_Head;

			if ( m_Head == NULL )
			{
				// just set tail to null incase
				m_Tail = NULL;
				// already empty
				return;
			}
			
			try
			{				
				tempNode = m_Head;
				
				if ( m_Tail == m_Head )
				{
					m_Tail = NULL;
					m_Head = NULL;
				}
				else 
					m_Head = m_Head->m_Next;
				
				tempNode->_delete();
				delete tempNode;
				
			}
			
			catch ( ... )
			{
				m_Head = NULL;
				m_Tail = NULL;

				BugMessage(NULL,"Bad pointer in queue, (Resetting queue) Memory may have been lost");
			}
		}

		/*BOOL IsMemberByPointer ( const T *pObj )
		{
			dataNode<T> *tempNode = m_Head;

			while ( tempNode )
			{
				if ( tempNode->m_NodeData == *pObj )
				{
					// dont want program to free tempNode, already used in stack.
					tempNode = NULL;

					return TRUE;
				}

				tempNode = tempNode->m_Next;
			}

			return FALSE;
		}*/

		BOOL IsMember ( const T pObj )
		{
			dataNode<T> *tempNode = m_Head;

			while ( tempNode )
			{
				if ( tempNode->m_NodeData == pObj )
				{
					// dont want program to free tempNode, already used in stack.
					tempNode = NULL;

					return TRUE;
				}

				tempNode = tempNode->m_Next;
			}

			return FALSE;
		}

		/*void Remove ( T *tRemoved )
		{
			dataNode<T> *probeNode = m_Head;

			if ( m_Head == NULL )
				return;

			*tRemoved = m_Tail->m_NodeData;

			if ( m_Tail == m_Head )
			{
				free(m_Tail);

				m_Head = NULL;				
				m_Tail = NULL;
			}
			else
			{				
				while ( (probeNode != m_Tail) && (probeNode->m_Next != m_Tail ) )
					probeNode = probeNode->m_Next;
				
				probeNode->m_Next = NULL;
				
				free(m_Tail);
				
				m_Tail = probeNode;				
			}
		}*/

		BOOL Remove ( const T pObj )
		{
			dataNode<T> *tempNode = m_Head;			
			dataNode<T> *deleteNode = NULL;

			if ( m_Head == NULL )
				return FALSE;

			if ( m_Head->m_NodeData == pObj )
			{
				if ( m_Head == m_Tail )
				{
					m_Tail = NULL;
					m_Head = NULL;
				}
				else
				{
					m_Head = m_Head->m_Next;
				}

				tempNode->_delete();
				delete tempNode;

				tempNode = NULL;

				return TRUE;
			}

			while ( tempNode && tempNode->m_Next )
			{
				if ( tempNode->m_Next->m_NodeData == pObj )
				{
					deleteNode = tempNode->m_Next;

					if ( deleteNode == m_Tail )
					{
						m_Tail = tempNode;
						tempNode->m_Next = NULL;
					}
					else
						tempNode->m_Next = deleteNode->m_Next;

					delete deleteNode;
					
					tempNode = NULL;
					deleteNode = NULL;

					return TRUE;
				}

				tempNode = tempNode->m_Next;
			}

			return FALSE;
		}

		BOOL RemoveByPointer ( const T *pObj )
		{
			dataNode<T> *tempNode = m_Head;			
			dataNode<T> *deleteNode = NULL;

			if ( m_Head == NULL )
				return FALSE;

			if ( &m_Head->m_NodeData == pObj )
			{
				if ( m_Head != m_Tail )
				{
					m_Head = m_Head->m_Next;
				}
				else
				{
					m_Head = NULL;
					m_Tail = NULL;
				}

				delete tempNode;
				tempNode = NULL;

				return TRUE;
			}

			while ( tempNode && tempNode->m_Next )
			{
				if ( &tempNode->m_Next->m_NodeData == pObj )
				{
					deleteNode = tempNode->m_Next;

					if ( deleteNode == m_Tail )
					{
						m_Tail = tempNode;
						tempNode->m_Next = NULL;
					}
					else
						tempNode->m_Next = deleteNode->m_Next;

					deleteNode->_delete();
					delete deleteNode;

					tempNode = NULL;
					deleteNode = NULL;

					return TRUE;
				}

				tempNode = tempNode->m_Next;
			}

			return FALSE;
		}
/*
		void Remove ( const T &ToRemove )
		{
			dataNode<T> *m_Temp;
			dataNode<T> *m_Free;
			
			if ( !m_Head )
				return; // Can't remove anything (empty)

			m_Temp = m_Head;

			while ( m_Temp != NULL )
			{
				if ( m_Temp->m_NodeData == ToRemove )
				{
					m_Free = m_Head;
					
					m_Head = m_Head->m_Next;
					
					delete m_Free;

					return;
				}		
				else if ( m_Temp->m_Next )
				{
					if ( m_Temp->m_Next->m_NodeData )
					{
						m_Free = m_Temp->m_Next;

						if ( m_Free == m_Tail )
							m_Tail = m_Temp;
						
						m_Temp->m_Next = m_Temp->m_Next->m_Next;
						
						delete m_Free;

						return;
					}
				}
			}
		}*/

	private:

		dataNode<T> *m_Head;
		dataNode<T> *m_Tail;

};

template <class T>
class dataStack
{
	public:

		void Init ( void )
		{
			m_Head = NULL;
		}

		dataStack()// constructor -- Must be public.
		{
			Init();			
		}

		/*~dataStack()
		{
			// Might cause problems with temporary stacks??
			// make sure head is null after finishing with temp stacks!
			this->Destroy();
		}*/

		void _delete ()
		{
			this->Destroy();
		}

		BOOL IsMember ( const T pObj )
		{
			dataNode<T> *tempNode = m_Head;

			while ( tempNode )
			{
				if ( tempNode->m_NodeData == pObj )
				{
					// dont want program to free tempNode, already used in stack.
					tempNode = NULL;
					return TRUE;
				}

				tempNode = tempNode->m_Next;
			}

			return FALSE;
		}

		BOOL RemoveByPointer ( const T *pObj )
		{
			dataNode<T> *tempNode = m_Head;			
			dataNode<T> *deleteNode;

			if ( m_Head == NULL )
				return FALSE;

			if ( &m_Head->m_NodeData == pObj )
			{
				m_Head = m_Head->m_Next;

				delete tempNode;

				return TRUE;
			}

			while ( tempNode && tempNode->m_Next )
			{
				if ( &tempNode->m_Next->m_NodeData == pObj )
				{
					deleteNode = tempNode->m_Next;

					tempNode->m_Next = tempNode->m_Next->m_Next;

					delete deleteNode;

					// dont want to free tempnode either!
					tempNode = NULL;					
					deleteNode = NULL;

					return TRUE;
				}

				tempNode = tempNode->m_Next;
			}

			return FALSE;
		}

		BOOL Remove ( const T pObj )
		{
			dataNode<T> *tempNode = m_Head;			
			dataNode<T> *deleteNode;

			if ( m_Head == NULL )
				return FALSE;

			if ( m_Head->m_NodeData == pObj )
			{
				m_Head = m_Head->m_Next;

				delete tempNode;

				return TRUE;
			}

			while ( tempNode && tempNode->m_Next )
			{
				if ( tempNode->m_Next->m_NodeData == pObj )
				{
					deleteNode = tempNode->m_Next;

					tempNode->m_Next = tempNode->m_Next->m_Next;

					delete deleteNode;

					tempNode = NULL;
					deleteNode = NULL;

					return TRUE;
				}

				tempNode = tempNode->m_Next;
			}

			return FALSE;
		}

		void Destroy ( void )
		{
			dataNode<T> *tempNode;

			while ( m_Head )
			{
				tempNode = m_Head;

				m_Head = m_Head->m_Next;

				tempNode->_delete();
				delete tempNode;

				tempNode = NULL;
			}

			m_Head = NULL;
		}

		inline BOOL IsEmpty ( void )
		{
			return ( m_Head == NULL );
		}

		void Push ( const T pObj )
		{
			dataNode<T> *newNode = new dataNode<T>;

			newNode->m_NodeData = pObj;
			newNode->m_Next = m_Head;

			m_Head = newNode;

		}

		T Pop ( void )
		{
			dataNode<T> *tempNode = m_Head;

			T returnData = tempNode->m_NodeData;

			m_Head = m_Head->m_Next;
			delete tempNode;

			return returnData;
		}

		T ChooseFromStack ( void )
		{					
			T *l_pTemp;

			try
			{
				l_pTemp = &m_Head->m_NodeData;		
				
				m_Head = m_Head->m_Next;
			}

			catch ( ... )
			{
				// problem
				BugMessage(NULL,"Bad pointer in stack, (Resetting tasks) Memory may have been lost");

				m_Head = NULL;
				// return default
				return T();
			}

			return *l_pTemp;
		}

		T *ChoosePointerFromStack ( void )
		{
			T *l_pTemp;

			try
			{
				l_pTemp = &m_Head->m_NodeData;
				
				m_Head = m_Head->m_Next;
			}

			catch ( ... )
			{
				// problem
				BugMessage(NULL,"Bad pointer in stack, (Resetting tasks) Memory may have been lost");

				m_Head = NULL;
				// return default
				return NULL;
			}

			return l_pTemp;
		}

		// Returns a pointer to the DETAILS (m_NodeData)
		// in the HEAD.
		T *GetHeadInfoPointer ( void )
		{
			if ( m_Head )
			{
				return &m_Head->m_NodeData;
			}

			return NULL;
		}

	private:

		dataNode<T> *m_Head;
};

#define DEFAULT_ARRAY_VALUE 1

template <class T>
class dataArray
{
	public:
		
		dataArray( )
		{
			this->Init();
		}
		
		~dataArray()
		{
			this->Clear();
		}

		inline void Clear ( void )
		{
			free(m_pArray);
			this->Init();
		}

		inline void Init ( void )
		{
			m_pArray = NULL;
			m_iArrayMax = 0;
		}

		void SetSize(int iArrayMax)
		{
			int iSize;

			assert(iArrayMax > 0);
			
			if ( iArrayMax <= 0 )
				iArrayMax = DEFAULT_ARRAY_VALUE;
			
			//iArrayMax ++;

			iSize = sizeof(T) * iArrayMax;
			
			m_pArray = (T*)malloc(iSize);
			
			assert(m_pArray != NULL);
			
			if ( m_pArray )
			{
				memset(m_pArray, 0, iSize);

				m_iArrayMax = iArrayMax;
			}
			else
			{
				// Couldnt allocate memory...
				m_iArrayMax = 0;
			}
		}
		
		T &operator [] (int iIndex) 
		{
			assert(m_pArray != NULL);
			assert(iIndex >= 0);
			assert(iIndex < m_iArrayMax);

			if ( ( m_pArray != NULL ) && ( (iIndex >= 0) && (iIndex < m_iArrayMax) ) )
				return m_pArray[iIndex];
			else 
			{
				// ALERT MESSAGE!!!
				return m_pArray[0]; // There is going to be at least 1 item, return the first for safety
			}
		}

	private:
		
		T *m_pArray;
		int m_iArrayMax;		
};
/*
template <class T>
class dataUnconstArray
{
	public:
		
		dataUnconstArray( )
		{
			this->Init();
		}
		
		~dataUnconstArray()
		{
			this->Clear();
		}

		inline Size (void)
		{
			return m_iArrayMax;
		}

		inline void Destroy (void)
		{
			delete m_pArray;			
		}

		inline void Init ( void )
		{
			m_pArray = NULL;
			m_iArrayMax = -1;
		}

		inline BOOL IsEmpty ( void )
		{
			return (m_pArray == NULL);
}

		inline void Clear ( void )
		{
			this->Destroy();
			this->Init();
		}

		T Random ( void )
		{
			return m_pArray[RANDOM_LONG(0,m_iArrayMax-1)];
		}

		void Add ( const T pObj )
		{
			if ( m_pArray == NULL )
			{
				m_pArray = new T;//(T*)malloc(sizeof(T));
				m_pArray[0] = pObj;
				m_iArrayMax = 1;
			}
			else
			{			
				T *temp = new T[m_iArrayMax+1];

				memcpy(temp,m_pArray,sizeof(T)*m_iArrayMax);

				temp[m_iArrayMax] = pObj;
				m_iArrayMax++;

				delete m_pArray;

				m_pArray = temp;

			}
		}
		


		T ReturnValueFromIndex ( int iIndex )
		{
			assert(m_pArray != NULL);
			assert(iIndex >= 0);
			assert(iIndex < m_iArrayMax);

			if ( ( m_pArray != NULL ) && ( (iIndex >= 0) && (iIndex < m_iArrayMax) ) )
				return m_pArray[iIndex];

			BotMessage(NULL,0,"dataUnconstArray::ReturnValueFromIndex() Error !");

			// return first array index
			return T(NULL);
		}

		BOOL IsMember ( T Obj )
		{
			int i;

			if ( m_pArray == NULL )
				return FALSE;

			for ( i = 0; i < m_iArrayMax; i ++ )
			{
				if ( m_pArray[i] == Obj )
					return TRUE;
			}

			return FALSE;
		}

		int m_iArrayMax;
		
	protected:
		T *m_pArray;		
};*/

template <class T>
class dataUnconstArray
{
	public:
		
		dataUnconstArray( )
		{
			this->Init();
		}
		
		~dataUnconstArray()
		{
			this->Clear();
		}

		inline int Size (void)
		{
			return array.size();
		}

		void Remove ( T obj )
		{
			if ( array.size() == 0  )
				return;
#if defined(_WIN32) && !defined(__CYGWIN__)
			vector<T>::iterator it;
#else
			/*anonym001*/
			typename vector<T>::iterator it;
#endif
			for ( it = array.begin(); it != array.end(); ++ it )
			{
				if ( *it == obj )
				{
					array.erase(it);
					return;
				}
			}
		}

		void RemoveByPointer ( T *obj )
		{
			if ( array.size() == 0  )
				return;

#if defined(_WIN32) && !defined(__CYGWIN__)
			vector<T>::iterator it;
#else
			/*anonym001*/
			typename vector<T>::iterator it;
#endif

			for ( it = array.begin(); it != array.end(); ++ it )
			{
				/* pointer comparison only */
//#if defined(_WIN32) && !defined(__CYGWIN__)
				//if ( it == obj )
//#else
				if ( &*it == obj )
//#endif
				{
					array.erase(it);
					return;
				}
			}
		}

		inline void Destroy (void)
		{
			array.clear();
		}

		inline void Init ( void )
		{
			array.clear();
		}

		inline bool IsEmpty ( void )
		{
			return array.empty();
		}

		inline void Clear ( void )
		{
			this->Destroy();
			this->Init();
		}

		T Random ( void )
		{
			return array[RANDOM_LONG(0,array.size()-1)];
		}

		void Add ( const T pObj )
		{
			array.push_back(pObj);
		}

		T ReturnValueFromIndex ( int iIndex )
		{
			return array[iIndex];
		}

		T* ReturnPointerFromIndex ( int iIndex )
		{
			return &(array[iIndex]);
		}

		T &operator [] ( int iIndex )
		{
			if ( (iIndex >= Size()) || (iIndex < 0) )
			{
				throw "[RCBOT>] dataUnconstarray[] Array exception: index out of bounds";
			}

			return (array[iIndex]);
		}

		T *getExisting ( T Obj )
		{
			for ( unsigned int i = 0; i < array.size(); i ++ )
			{
				if ( array[i] == Obj )
					return &(array[i]);
			}

			return NULL;
		}

		BOOL IsMember ( T Obj )
		{
			for ( unsigned int i = 0; i < array.size(); i ++ )
			{
				if ( array[i] == Obj )
					return TRUE;
			}

			return FALSE;
		}
	private:
		vector<T> array;
};
/*
#define D_HASH_TABLE_SETS 1
template <class T>
class dataHashTable
{
	public:
		dataHashTable()
		{
			int i;

			for ( i = 0; i < D_HASH_TABLE_SETS; i++ )
				m_Lists[i].Init();
		}

		void Add ( int iHashValue, T pNewObj )
		{
			int iHashNum = iHashValue % D_HASH_TABLE_SETS;

			assert(iHashNum>=0);
			assert(iHashNum<D_HASH_TABLE_SETS);

			m_Lists[iHashNum].Push(pNewObj);
		}

		void ProcessHashTableHashNum ( void (*pProcessFunction)(const dataStack<T>), int iHashValue )
		{
			int iHashNum = iHashValue % D_HASH_TABLE_SETS;

			assert(iHashNum>=0);
			assert(iHashNum<D_HASH_TABLE_SETS);

			(*pProcessFunction)(m_Lists[iHashNum]);
		}

		void ProcessHashTable ( void (*pProcessFunction)(const dataStack<T>) )
		{
			int i;

			for ( i = 0; i < D_HASH_TABLE_SETS; i ++ )
				(*pProcessFunction)(m_Lists[i]);
		}

	private:

		dataStack<T> m_Lists[D_HASH_TABLE_SETS];
};
*/

#endif