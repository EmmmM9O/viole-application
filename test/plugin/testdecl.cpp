class foo {
  virtual void test1();
  virtual void test2();
};
class bar : public foo {
  void test1() override;
};
