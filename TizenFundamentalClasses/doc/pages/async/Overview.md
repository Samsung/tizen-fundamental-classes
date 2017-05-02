[&uarr;<SUB>Back to top</SUB>](#top)

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
[&uarr;<SUB>Back to top</SUB>](#top)
@section tfc-async-syntax Syntax

<table>
	<tr>
		<td> __tfc_async__ `{` *asynchronous-body* `}` __tfc_async_complete__ `{` *completion-body* `};`</td>
		<td>(1)</td>
	</tr>
	<tr>
		<td> *lvalue-assignment* <SUB>(optional)</SUB> __tfc_async__ `{` *asynchronous-body* `}` >> *shared-event-object*`;`</td>
		<td>(2)</td>
	</tr>
	<tr>
		<td> *lvalue-assignment* <SUB>(optional)</SUB> __tfc_async__ `{` *asynchronous-body* `};`</td>
		<td>(3)</td>
	</tr>
</table>
1. Asynchronous operation with completion block.
2. Asynchronous operation with event-based completion callback.
3. Asynchronous operation without automatic completion callback.
@subsection tfc-async-syntax-explanation Explanation

<table>
	<tr>
		<td class="col-expl-item">asynchronous-body</td>
		<td class="col-expl-arrow">&rarr;</td>
		<td class="col-expl-desc">
			statements that will be executed asynchronously
		</td>
	</tr>
	<tr>
		<td class="col-expl-item">completion-body</td>
		<td class="col-expl-arrow">&rarr;</td>
		<td class="col-expl-desc">
			statements that will be executed synchronously after *asynchronous-statements* execution
			completes
		</td>
	</tr>
	<tr>
		<td class="col-expl-item">lvalue-assignment</td>
		<td class="col-expl-arrow">&rarr;</td>
		<td class="col-expl-desc">
			assignment of __tfc_async__ result value to lvalue storage with type of pointer to 
			TFC::Async<T>::Task
		</td>
	</tr>
	<tr>
		<td class="col-expl-item">shared-event-object</td>
		<td class="col-expl-arrow">&rarr;</td>
		<td class="col-expl-desc">
			object of shared event which will the event will be raised after
			*asynchronous-statements* execution completes
		</td>
	</tr>
</table>

[&uarr;<SUB>Back to top</SUB>](#top)
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
[&uarr;<SUB>Back to top</SUB>](#top)
@section tfc-async-event Notifying Completion via Event

Developers can utilize event to receive completion notification of asynchronous operation. Program
can declare a function with specific signature which can be subscribe to event that listen to
asynchronous completion.

TAL provides TFC::Async which is a template-helper class which declares necessary types related with
asynchronous operation. Declare an event with TFC::Async<T>::Event type where template parameter `T`
must be substituted with the return type from asynchronous block.

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

TFC::Async<T>::Event utilizes a std::shared_ptr as its backing to ensure that the event will live
beyond the caller's lifetime, to address issues when using basic event type (i.e.
TFC::EventEmitterClass<T>::Event) which lives in the same allocation with the caller's object. This
creates issues if the caller's object is deleted before the asynchronous process is completed, which
will make the asynchronous try to call deleted object.
[&uarr;<SUB>Back to top</SUB>](#top)

@section tfc-async-handle Using Task Handle

tfc_async returns a task handle which can be used to refer the running task. The task handle type
is pointer to TFC::Async<TReturnValue>::Task. Task handle is used to identify asynchronous type and 
will remain valid as long as the thread is running.

Task handle can be used to perform blocking tfc_await or non-blocking tfc_try_await. Those operation 
can only be performed if tfc_async is called without completion handler. Task handle can also useful 
to identify running task on event-based completion handler, if a handler is supposed to handle
completion of multiple running task. Using `tfc_await` and `tfc_try_await` in a `tfc_async` with 
completion handler will result in undefined behavior.

__Example 4: Using task handle on `tfc_async`:__
```{.cpp}
class TheClass
{
private:
    TFC::Async<std::string>::Task taskCompute;
public:
    void RunTask()
    {
        this->taskCompute = tfc_async {
            return std::string("Result from task");
        };
    }
    
	void BlockIfNotCompleted()
	{
		// The statement below will block until result is ready
		std::string result = tfc_await taskCompute;
		
		// At this point, result is ready and async operation is completed
		evas_object_text_set(button, result.c_str());
	}
};
```

Using `tfc_async` without `tfc_async_complete` clause will render the asynchronous operation to
complete without cleaning up its infrastructure. Developers must explicitly call `tfc_await` or
`tfc_try_await` for that task to explicitly acknowledge that asynchronous operation is completed.
Developers should not spawn and discard asynchronous operation. Should you don't need completion
operation, you must make empty `tfc_async_complete` clause instead to ensure the asynchronous
infrastructure is cleaned up gracefully.

Moreover, `tfc_await` is a blocking instruction, so calling it on main thread will block it and
makes the application unresponsive. You should use `tfc_try_await` if you intend to use this 
mechanism in main thread. `tfc_await` is more suitable to be used in another asynchronous block to 
join execution between thread.
[&uarr;<SUB>Back to top</SUB>](#top)

@section tfc-async-scope Scope Behavior

TAL utilizes C++ lambda closure to provide its syntactic sugar in constructing asynchronous
operation in codes. The closure block captures all variable in the enclosing block by value, thus
it is recommended to keep the number of stack allocated object low to minimize copy operation from
the enclosing block to the lambda closure.

To utilize complex data (i.e. class object) efficiently, allocate the class in heap. It is not
necessary but recommended to wrap the heap allocated data in automatic pointer std::shared_ptr to
ensure the heap allocated object is properly destroyed when it is not used anymore.

Allocating std::unique_ptr in enclosing block does not work because std::unique_ptr cannot be 
copied,  instead it uses move semantics. It is okay to use std::unique_ptr inside asynchronous block
by supplying pointer to object allocated in the enclosing block to ensure the dynamically allocated 
object is properly destroyed after the control leaves asynchronous block.

Statements after `tfc_async` clauses will be automatically executed after `tfc_async` clause
completes registering the asyncrhonous operation internally. It will be executed asynchronously and
may be in parallel with the execution of asynchronous block. Developers must not put assumption
on which execution goes first and should use syncrhonization mechanism such as locking to prevent
race condition. `tfc_await` provides indefinite locking until the asynchronous thread completes its
entire execution (i.e. leave the asynchronous block).