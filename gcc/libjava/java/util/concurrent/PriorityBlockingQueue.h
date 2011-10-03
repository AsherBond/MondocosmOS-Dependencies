
// DO NOT EDIT THIS FILE - it is machine generated -*- c++ -*-

#ifndef __java_util_concurrent_PriorityBlockingQueue__
#define __java_util_concurrent_PriorityBlockingQueue__

#pragma interface

#include <java/util/AbstractQueue.h>
#include <gcj/array.h>


class java::util::concurrent::PriorityBlockingQueue : public ::java::util::AbstractQueue
{

public:
  PriorityBlockingQueue();
  PriorityBlockingQueue(jint);
  PriorityBlockingQueue(jint, ::java::util::Comparator *);
  PriorityBlockingQueue(::java::util::Collection *);
  virtual jboolean add(::java::lang::Object *);
  virtual jboolean offer(::java::lang::Object *);
  virtual void put(::java::lang::Object *);
  virtual jboolean offer(::java::lang::Object *, jlong, ::java::util::concurrent::TimeUnit *);
  virtual ::java::lang::Object * poll();
  virtual ::java::lang::Object * take();
  virtual ::java::lang::Object * poll(jlong, ::java::util::concurrent::TimeUnit *);
  virtual ::java::lang::Object * peek();
  virtual ::java::util::Comparator * comparator();
  virtual jint size();
  virtual jint remainingCapacity();
  virtual jboolean remove(::java::lang::Object *);
  virtual jboolean contains(::java::lang::Object *);
  virtual JArray< ::java::lang::Object * > * toArray();
  virtual ::java::lang::String * toString();
  virtual jint drainTo(::java::util::Collection *);
  virtual jint drainTo(::java::util::Collection *, jint);
  virtual void clear();
  virtual JArray< ::java::lang::Object * > * toArray(JArray< ::java::lang::Object * > *);
  virtual ::java::util::Iterator * iterator();
private:
  void writeObject(::java::io::ObjectOutputStream *);
public: // actually package-private
  static ::java::util::concurrent::locks::ReentrantLock * access$0(::java::util::concurrent::PriorityBlockingQueue *);
  static ::java::util::PriorityQueue * access$1(::java::util::concurrent::PriorityBlockingQueue *);
private:
  static const jlong serialVersionUID = 5595510919245408276LL;
  ::java::util::PriorityQueue * __attribute__((aligned(__alignof__( ::java::util::AbstractQueue)))) q;
  ::java::util::concurrent::locks::ReentrantLock * lock;
  ::java::util::concurrent::locks::Condition * notEmpty;
public: // actually package-private
  static jboolean $assertionsDisabled;
public:
  static ::java::lang::Class class$;
};

#endif // __java_util_concurrent_PriorityBlockingQueue__