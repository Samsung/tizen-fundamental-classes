@page tfc-async %TFC Asynchronous Library (TAL)
@tableofcontents
@section tfc-async-overview Overview

%TFC Asynchronous Library (TAL) is provided to provide smooth interfacing between C++ code and EFL's
threading components. Due to the infrastructure of EFL threading and event loop, it is currently not
possible to utilize C++'s standard asynchronous infrastructure such as `std::async` inside Tizen's
C++ code.

TAL introduces several keywords which can be used to introduce asynchronous block in code. The
keyword utilizes C++ lambda closure to work then pass the closure to backend codes which utilizes
EFL's Ecore threading functions. Developers does not need to know the entire mechanism how TAL
works behind and can focus on writing application codes instead.

@section tfc-async-basic Basic Usage

To utilize TAL in code, developers can declare asynchronous block using `tfc_async` keyword. This
keyword introduces asynchronous block where the code block is processed on separate thread which 
does not block the main thread. `tfc_async` block will immediately queued to be executed as soon
as the TAL statements executed. The actual execution may vary according to the available thread
in Ecore threading infrastructure. Developers may not assume the thread state, using
synchronizing mechanism required, such as using Mutex as semaphore (`std::mutex`).

TAL provides keyword `tfc_async_complete` which introduces a block that will be executed right after
the asynchronous block is completed. It is guaranteed that `tfc_async_complete` block will be called
synchronously with the main thread, so it is safe to call EFL and Elementary functions which are
generally non-thread-safe. Developers must ensure that the function using `tfc_async` to conclude 
the execution trivially and should not wait indefinitely for signal from asynchronous block as it
will block the main thread.

Due to the nature of lambda closure and C++ expression statement to build this mechanism,
unfortunately every tfc_async declaration must be concluded with an awkward semicolon.

__Example 1: usage with `tfc_async_complete` block__
```{.cpp}
void ListController::PerformLongOperation()
{
   tfc_async
   {
       this->PerformHttpRequestWithCurl();
   }
   tfc_async_complete
   {
       evas_object_text_set(labelStatus, "Asynchronous completed!");
   };
}
```

`tfc_async` block can pass result value from asynchronous operation by return keyword. The value can 
be retrieved by `tfc_async_complete` clause by declaring receiving parameter the same type with the 
value returned by `tfc_async` block. The syntax is similar fashion with `catch` clause in C++ 
exception handling.

__Example 2: usage with returning values and receive it in `tfc_async_complete` clause__
```{.cpp}
void ListController::PerformLongOperation()
{
   tfc_async
   {
       int a = rand();
       int b = rand();
       return a * b;
   }
   tfc_async_complete (int result)
   {
       if(result < 10000) 
           evas_object_text_set(labelStatus, "Result below 10000!");
       else
           evas_object_text_set(labelStatus, "Result above 10000!");
   };
}
```
[Back to top](#top)
@section tfc-async-event Notifying Completion via Event

Developers can utilize event to receive completion notification of asynchronous operation. Program
can declare a function with specific signature which can be subscribe to event that listen to
asynchronous completion.

TAL provides TFC::Async which is a template-helper class which declares necessary types related with
asynchronous operation. Declare an event with TFC::Async<TReturnValue>::Event type where template
parameter `TReturnValue` must be substituted with the return type from asynchronous block.

The signature for event handler function must be as follow:
```{.cpp}
void [FUNCTION NAME] (TFC::Async<T>::BaseEvent* ev, TFC::Async<T>::Task* t, T result);
```
Where once again `T` template parameter must be substituted with the respective return type from
asyncrhonous block.

__Example 3: Usage with event-based completion handler__
```{.cpp}
class TheClass
{
private:
    Evas_Object* labelStatus;
    TFC::Async<int>::Event eventAsyncCompleted;

    void OnAsyncCompleted(TFC::Async<int>::BaseEvent* ev, TFC::Async<int>::Task* t, int res)
    {
        evas_object_text_set(labelStatus, "Async completed!");
    }

public:
    TheClass()
    {
        this->eventAsyncCompleted += EventHandler(TheClass::OnAsyncCompleted);
    }

    void RunTask()
    {
        tfc_async
        {
            int val = rand();
            return rand3;
        } >> eventAsyncCompleted;
    }
};
```

TFC::Async::Event utilizes a std::shared_ptr as its backing to ensure that the event will live
beyond the caller's lifetime, to address issues when using basic event type (i.e.
TFC::EventEmitterClass::Event) which lives in the same allocation with the caller's object. This
creates issues if the caller's object is deleted before the asynchronous process is completed, which
will make the asynchronous try to call deleted object.

@section tfc-async-handle Using Task Handle

tfc_async returns a task handle which can be used to refer the running task. The task handle type
is pointer to TFC::Core::Async::AsyncTask or TFC::Async::Task as template trait equivalence.
Task handle is used to identify asynchronous type and will remain valid as long as the thread is
running.

Task handle can be used to perform blocking tfc_await or non-blocking tfc_try_await. Those operation 
can only be performed if tfc_async is called without completion handler. Task handle can also useful 
to identify running task on event-based completion handler, if a handler is supposed to handle
completion of multiple running task. Using `tfc_await` and `tfc_try_await` in a `tfc_async` with 
completion handler will result in undefined behavior.

Retrieving task handle on `tfc_async`:
```{.cpp}
class TheClass
{
private:
    TFC::Async<int>::Task taskCompute;
public:
    void RunTask()
    {
        this->taskCompute = tfc_async {
            int a = rand();
            return a2;
        };
    }
};
```