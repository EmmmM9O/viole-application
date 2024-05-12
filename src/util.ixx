module;
#include <any>
#include <type_traits>
#include <typeinfo>
export module viole_util;
export namespace viole {
template <typename T>
concept template_enum = std::is_enum<T>::value;
class bad_any_point : public std::exception {
  const char *what() const noexcept override;
};
class any_point {
  struct type_internal {
    type_internal() = default;
    virtual ~type_internal() = default;
    [[nodiscard]] virtual const std::type_info &get_type() const = 0;
    virtual void set_any(std::any data) = 0;
    virtual void set_void(void *data) = 0;
    virtual void set_void_ptr(void *data) = 0;
  };
  template <typename T> struct type_impl : public type_internal {
    T *_data = nullptr;

    explicit type_impl(T *data) : _data(data) {}

    [[nodiscard]] const std::type_info &get_type() const override {
      return typeid(T);
    }
    void set_any(std::any data) override { *_data = std::any_cast<T>(data); }
    void set_void(void *data) override { *_data = *(T *)data; }
    void set_void_ptr(void *data) override { _data = (T *)data; }
  };

public:
  [[nodiscard]] bool has_value() const;
  [[nodiscard]] const std::type_info &type() const;
  template <typename T> [[nodiscard]] bool same_as() const {
    return typeid(T).hash_code() == type().hash_code();
  }
  bool same_as(const std::any &data);
  template <typename T> void swap(T *point) {
    if (same_as<T>())
      _data->set_void_ptr(point);
    else
      throw bad_any_point();
  }
  void reset() noexcept;
  void set_any(std::any data);
  template <typename T> void set(T t) {
    if (same_as<T>())
      _data->set_void(&t);
    else
      throw bad_any_point();
  }
  any_point() = default;
  ~any_point() = default;
  template <typename T> any_point(T *point) : _data(new type_impl<T>(point)) {}

private:
  std::unique_ptr<type_internal> _data;
};

} // namespace viole
