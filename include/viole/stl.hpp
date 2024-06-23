#pragma once
#include "viole/base.hpp"
#include "viole/templates.hpp"
#include "viole/util.hpp"
#include <initializer_list>
#include <memory>
#include <type_traits>
namespace viole {
template <typename Data, typename Size>
class basic_queue : public basic_object {
  /*
   * index_type Size
   * data_type Data
   */
public:
  [[nodiscard]] virtual auto size() const -> Size = 0;
  [[nodiscard]] virtual auto empty() -> bool const = 0;
  virtual auto push(Data) -> void = 0;
  virtual auto clear() -> void = 0;
  virtual auto pop() -> Data = 0;
  [[nodiscard]] virtual auto front() const -> Data = 0;
  [[nodiscard]] virtual auto back() const -> Data = 0;
  [[nodiscard]] auto to_string() const noexcept -> viole::string override {
    return "{queue}";
  }
  [[nodiscard]] auto
  get_type() const noexcept -> const std::type_info & override {
    return typeid(basic_queue<Data, Size>);
  }
  ~basic_queue() override {  }
};
template <typename T>
concept basic_queue_template = std::is_base_of<basic_object, T>::value &&
  requires()
{
  typename T::index_type;
  typename T::data_type;
};

template <typename Data> class queue : public basic_queue<Data, std::size_t> {
  struct node;
  using node_ref = std::shared_ptr<node>;
  struct node {
    Data m_data;
    node_ref m_next = nullptr;
    node(Data m_data_, node_ref m_next_) : m_data(m_data_), m_next(m_next_) {}
  };
  node_ref m_front = nullptr, m_rear = nullptr;

public:
  [[nodiscard]] auto empty() -> bool const override { return m_size == 0; }
  [[nodiscard]] auto front() const -> Data override {
    if (m_front != nullptr) {
      return m_front->m_data;
    }
  }
  [[nodiscard]] auto back() const -> Data override {
    if (m_rear != nullptr) {
      return m_rear->m_data;
    }
  }
  auto pop() -> Data override {
    if (m_size != 0) {
      auto tmp = m_front;
      m_front = m_front->m_next;
      return tmp->m_data;
    }
  }

  auto clear() -> void override {
	  if(m_front==nullptr) return;
    for (auto handle = m_front; handle != nullptr; ) {
	    auto e=handle->m_next;
      handle.reset();
      if(e==nullptr) break;
      handle=e;
    }
    m_size = 0;
  }
  queue(const queue &) = delete;
  queue() : m_front(nullptr), m_rear(nullptr) {};
  queue(std::initializer_list<Data> list) {
    for (auto value : list) {
      push(value);
    }
  }
  ~queue() { 
  }
  using data_type = Data;
  using index_type = std::size_t;

  [[nodiscard]] auto size() const -> index_type override { return m_size; }
  auto push(Data data) -> void override {
    if (m_size == 0) {
      m_front = std::make_unique<node>(data, nullptr);
      m_rear = m_front;
    } else {
      m_rear->m_next = std::make_unique<node>(data, nullptr);
      m_rear = m_rear->m_next;
    }
    m_size++;
  }
  [[nodiscard]] auto to_string() const noexcept -> viole::string override {
    return "size:" + viole::to_string(m_size) + "{queue}";
  }
  [[nodiscard]] auto
  get_type() const noexcept -> const std::type_info & override {
    return typeid(queue<Data>);
  }

private:
  index_type m_size = 0;
};
TEMPLATE_FIT(queue<char>, basic_queue_template);
} // namespace viole
