namespace ccmqsrv {
template<typename T>
struct type_xx{	typedef T type; };

template<>
struct type_xx<void>{ typedef int8_t type; };

// 用tuple做参数调用函数模板类
template<typename Function, typename Tuple, std::size_t... Index>
inline decltype(auto) invoke_impl(Function&& func, Tuple&& t, std::index_sequence<Index...>)
{
	return func(std::get<Index>(std::forward<Tuple>(t))...);
}

template<typename Function, typename Tuple>
inline decltype(auto) invoke(Function&& func, Tuple&& t)
{
	constexpr auto size = std::tuple_size<typename std::decay<Tuple>::type>::value;
	return invoke_impl(std::forward<Function>(func), std::forward<Tuple>(t), std::make_index_sequence<size>{});
}

template<typename R, typename F, typename ArgsTuple>
inline typename std::enable_if<std::is_same<R, void>::value, typename type_xx<R>::type >::type
call_helper(F f, ArgsTuple args) {
	invoke(f, args);
	return 0;
}

template<typename R, typename F, typename ArgsTuple>
inline typename std::enable_if<!std::is_same<R, void>::value, typename type_xx<R>::type >::type
call_helper(F f, ArgsTuple args) {
	return invoke(f, args);
}

template <typename>
constexpr bool is_vector = false;

template <typename... Args>
constexpr bool is_vector<std::vector<Args...>> = true;

template<typename Tuple, std::size_t Id>
inline void getv(std::any &data, Tuple& t) {
  if (data.type().name()==typeid(typename std::tuple_element<Id, Tuple>::type).name()) {
    std::get<Id>(t) = std::any_cast<typename std::tuple_element<Id, Tuple>::type>(data);
  } 
  else {
    using arg_type = typename std::tuple_element<Id, Tuple>::type;
    if constexpr (!(std::is_same<arg_type, int>::value 
                 || std::is_same<arg_type, double>::value 
                 || std::is_same<arg_type, string>::value 
                 || std::is_same<arg_type, bool>::value 
                 || std::is_null_pointer<arg_type>::value)) 
    {
      typename std::tuple_element<Id, Tuple>::type t_;
      if constexpr (is_vector<arg_type>) {
        using value_type = typename std::tuple_element<Id, Tuple>::type::value_type;
        auto bad_any = std::any_cast<std::vector<std::any>>(data);
        std::transform(bad_any.begin(), bad_any.end(), std::back_inserter(t_),
        [](const std::any& value) {
          return std::any_cast<value_type>(value);
        });
      } else
      {
        using vt = typename std::tuple_element<Id, Tuple>::type::mapped_type;
        auto bad_any = std::any_cast<std::map<string, std::any>>(data);
        for (auto& [key, value] : bad_any) {
          t_[key] = std::any_cast<vt>(value);
        }
      }
      std::get<Id>(t) = t_;
    }
  }
}

template<typename Tuple, std::size_t... I>
inline Tuple get_tuple(std::vector<std::any> &data, std::index_sequence<I...>) {
  Tuple t;
  std::initializer_list<int>{((getv<Tuple, I>(data[I], t)), 0)...};
  return t;
}

// 函数指针
template<typename R, typename... Params>
inline void callproxy_(R(*func)(Params...), Res *res, std::vector<std::any> &data) {
  callproxy_(std::function<R(Params...)>(func), res, data);
}

// 成员函数指针
template<typename R, typename C, typename S, typename... Params>
inline void callproxy_(R(C::* func)(Params...), S* s, Res *res, std::vector<std::any> &data) {

  using args_type = std::tuple<typename std::decay<Params>::type...>;

  constexpr auto N = std::tuple_size<typename std::decay<args_type>::type>::value;
  args_type args = get_tuple<args_type>(data, std::make_index_sequence<N>{});

  auto ff = [=](Params... ps)->R {
    return (s->*func)(ps...);
  };
  typename type_xx<R>::type r = call_helper<R>(ff, args);
  res->result = json::array({r}).dump();
}

// func
template<typename R, typename... Params>
inline void callproxy_(std::function<R(Params... ps)> func, Res *res, std::vector<std::any> &data) {
  
  using args_type = std::tuple<typename std::decay<Params>::type...>;

  constexpr auto N = std::tuple_size<typename std::decay<args_type>::type>::value;
  args_type args = get_tuple<args_type>(data, std::make_index_sequence<N>{});
 
  typename type_xx<R>::type r = call_helper<R>(func, args);
  res->result = json::array({r}).dump();
}

template<typename Func>
inline void callproxy(Func f, Res *res, std::vector<std::any> &data)
{
  callproxy_(f, res, data);
}

template<typename Func, typename Self>
inline void callproxy(Func f, Self *self, Res *res, std::vector<std::any> &data)
{
  callproxy_(f, self, res, data);
}

template<typename Func>
inline void MessageQueueServer::register_rpc(const Func &func, const string &name)
{
  if (auto search = rpc_func_map.find(name); search == rpc_func_map.end())
  {
    rpc_func_map[name] = std::bind(&callproxy<Func>, func, _1, _2);
    spdlog::info("register_rpc: {}", name);
  }
  else {
    throw;
  }
}

template<typename Func, typename Self>
inline void MessageQueueServer::register_rpc(const Func &func, Self *self, const string &name) 
{
  if (auto search = rpc_func_map.find(name); search == rpc_func_map.end())
  {
    rpc_func_map[name] = std::bind(&callproxy<Func, Self>, func, self, _1, _2);
    spdlog::info("register_rpc: {}", name);
  }
  else {
    throw;
  }
}

template<typename CallBack>
inline void MessageQueueServer::register_event_handler(string event_type, CallBack cb)
{
  if (auto search = event_handler_map.find(event_type); search == event_handler_map.end())
  {
    event_handler_map[event_type] = {};
  }
  auto &handler_map = event_handler_map[event_type];
  this->evt_cb_id++;
  int id = this->evt_cb_id;
  assert(handler_map.find(id) == handler_map.end());
  handler_map[id] = std::bind(&callproxy<CallBack>, cb, _1, _2);
  spdlog::info("register_event_handler: {}", event_type);
}

template<typename CallBack, typename Self>
inline void MessageQueueServer::register_event_handler(string event_type, Self *self, CallBack cb)
{
  if (auto search = event_handler_map.find(event_type); search == event_handler_map.end())
  {
    event_handler_map[event_type] = {};
  }
  auto &handler_map = event_handler_map[event_type];
  this->evt_cb_id++;
  int id = this->evt_cb_id;
  assert(handler_map.find(id) == handler_map.end());
  handler_map[id] = std::bind(&callproxy<CallBack, Self>, cb, self, _1, _2);
  spdlog::info("register_event_handler: {}", event_type);
}

} // namespace ccmqsrv