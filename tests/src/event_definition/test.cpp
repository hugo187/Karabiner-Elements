#define CATCH_CONFIG_RUNNER
#include "../../vendor/catch/catch.hpp"

#include "manipulator/details/types.hpp"
#include "thread_utility.hpp"
#include <boost/optional/optional_io.hpp>

namespace {
auto a_key_code = *(krbn::types::get_key_code("a"));
auto escape_key_code = *(krbn::types::get_key_code("escape"));
auto left_shift_key_code = *(krbn::types::get_key_code("left_shift"));
auto right_option_key_code = *(krbn::types::get_key_code("right_option"));
auto right_shift_key_code = *(krbn::types::get_key_code("right_shift"));
auto spacebar_key_code = *(krbn::types::get_key_code("spacebar"));
auto tab_key_code = *(krbn::types::get_key_code("tab"));

krbn::modifier_flag_manager::active_modifier_flag left_command_1(krbn::modifier_flag_manager::active_modifier_flag::type::increase,
                                                                 krbn::modifier_flag::left_command,
                                                                 krbn::device_id(1));
krbn::modifier_flag_manager::active_modifier_flag left_control_1(krbn::modifier_flag_manager::active_modifier_flag::type::increase,
                                                                 krbn::modifier_flag::left_control,
                                                                 krbn::device_id(1));
krbn::modifier_flag_manager::active_modifier_flag left_option_1(krbn::modifier_flag_manager::active_modifier_flag::type::increase,
                                                                krbn::modifier_flag::left_option,
                                                                krbn::device_id(1));
krbn::modifier_flag_manager::active_modifier_flag left_shift_1(krbn::modifier_flag_manager::active_modifier_flag::type::increase,
                                                               krbn::modifier_flag::left_shift,
                                                               krbn::device_id(1));
krbn::modifier_flag_manager::active_modifier_flag right_shift_1(krbn::modifier_flag_manager::active_modifier_flag::type::increase,
                                                                krbn::modifier_flag::right_shift,
                                                                krbn::device_id(1));
} // namespace

TEST_CASE("event_definition.get_modifier") {
  using krbn::manipulator::details::event_definition;

  REQUIRE(event_definition::get_modifier(krbn::modifier_flag::zero) == event_definition::modifier::end_);
  REQUIRE(event_definition::get_modifier(krbn::modifier_flag::left_shift) == event_definition::modifier::left_shift);
}

TEST_CASE("event_definition.test_modifier") {
  using krbn::manipulator::details::event_definition;

  {
    krbn::modifier_flag_manager modifier_flag_manager;
    modifier_flag_manager.push_back_active_modifier_flag(left_command_1);

    {
      auto actual = event_definition::test_modifier(modifier_flag_manager, event_definition::modifier::left_shift);
      auto expected = std::make_pair(false, krbn::modifier_flag::zero);
      REQUIRE(actual == expected);
    }
    {
      auto actual = event_definition::test_modifier(modifier_flag_manager, event_definition::modifier::left_command);
      auto expected = std::make_pair(true, krbn::modifier_flag::left_command);
      REQUIRE(actual == expected);
    }
    {
      auto actual = event_definition::test_modifier(modifier_flag_manager, event_definition::modifier::right_command);
      auto expected = std::make_pair(false, krbn::modifier_flag::zero);
      REQUIRE(actual == expected);
    }
    {
      auto actual = event_definition::test_modifier(modifier_flag_manager, event_definition::modifier::command);
      auto expected = std::make_pair(true, krbn::modifier_flag::left_command);
      REQUIRE(actual == expected);
    }
    {
      auto actual = event_definition::test_modifier(modifier_flag_manager, event_definition::modifier::shift);
      auto expected = std::make_pair(false, krbn::modifier_flag::zero);
      REQUIRE(actual == expected);
    }
  }
  {
    krbn::modifier_flag_manager modifier_flag_manager;
    modifier_flag_manager.push_back_active_modifier_flag(right_shift_1);

    {
      auto actual = event_definition::test_modifier(modifier_flag_manager, event_definition::modifier::left_shift);
      auto expected = std::make_pair(false, krbn::modifier_flag::zero);
      REQUIRE(actual == expected);
    }
    {
      auto actual = event_definition::test_modifier(modifier_flag_manager, event_definition::modifier::right_shift);
      auto expected = std::make_pair(true, krbn::modifier_flag::right_shift);
      REQUIRE(actual == expected);
    }
    {
      auto actual = event_definition::test_modifier(modifier_flag_manager, event_definition::modifier::shift);
      auto expected = std::make_pair(true, krbn::modifier_flag::right_shift);
      REQUIRE(actual == expected);
    }
  }
}

TEST_CASE("event_definition.test_modifiers") {
  using krbn::manipulator::details::event_definition;

  {
    std::unordered_set<event_definition::modifier> modifiers;
    event_definition event_definition(spacebar_key_code, modifiers);

    REQUIRE(*(event_definition.get_key_code()) == spacebar_key_code);
    REQUIRE(event_definition.get_modifiers() == modifiers);

    {
      krbn::modifier_flag_manager modifier_flag_manager;
      REQUIRE(*event_definition.test_modifiers(modifier_flag_manager) == std::unordered_set<krbn::modifier_flag>({}));
    }
    {
      krbn::modifier_flag_manager modifier_flag_manager;
      modifier_flag_manager.push_back_active_modifier_flag(left_shift_1);
      REQUIRE(event_definition.test_modifiers(modifier_flag_manager) == boost::none);
    }
  }

  {
    std::unordered_set<event_definition::modifier> modifiers;
    event_definition event_definition(left_shift_key_code, modifiers);

    REQUIRE(*(event_definition.get_key_code()) == left_shift_key_code);
    REQUIRE(event_definition.get_modifiers() == modifiers);

    {
      krbn::modifier_flag_manager modifier_flag_manager;
      REQUIRE(*event_definition.test_modifiers(modifier_flag_manager) == std::unordered_set<krbn::modifier_flag>({}));
    }
    {
      krbn::modifier_flag_manager modifier_flag_manager;
      modifier_flag_manager.push_back_active_modifier_flag(left_shift_1);
      if (event_definition.test_modifiers(modifier_flag_manager) != boost::none) {
        REQUIRE(false);
      }
    }
  }

  {
    std::unordered_set<event_definition::modifier> modifiers({
        event_definition::modifier::any,
    });
    event_definition event_definition(spacebar_key_code, modifiers);

    REQUIRE(*(event_definition.get_key_code()) == spacebar_key_code);
    REQUIRE(event_definition.get_modifiers() == modifiers);

    {
      krbn::modifier_flag_manager modifier_flag_manager;
      REQUIRE(*event_definition.test_modifiers(modifier_flag_manager) == std::unordered_set<krbn::modifier_flag>({}));
    }
    {
      krbn::modifier_flag_manager modifier_flag_manager;
      modifier_flag_manager.push_back_active_modifier_flag(left_shift_1);
      REQUIRE(*event_definition.test_modifiers(modifier_flag_manager) == std::unordered_set<krbn::modifier_flag>({}));
    }
    {
      krbn::modifier_flag_manager modifier_flag_manager;
      modifier_flag_manager.push_back_active_modifier_flag(left_command_1);
      modifier_flag_manager.push_back_active_modifier_flag(left_shift_1);
      REQUIRE(*event_definition.test_modifiers(modifier_flag_manager) == std::unordered_set<krbn::modifier_flag>({}));
    }
  }

  {
    std::unordered_set<event_definition::modifier> modifiers({
        event_definition::modifier::left_shift,
    });
    event_definition event_definition(spacebar_key_code, modifiers);

    REQUIRE(*(event_definition.get_key_code()) == spacebar_key_code);
    REQUIRE(event_definition.get_modifiers() == modifiers);

    {
      krbn::modifier_flag_manager modifier_flag_manager;
      modifier_flag_manager.push_back_active_modifier_flag(left_command_1);
      if (event_definition.test_modifiers(modifier_flag_manager) != boost::none) {
        REQUIRE(false);
      }
    }
    {
      krbn::modifier_flag_manager modifier_flag_manager;
      modifier_flag_manager.push_back_active_modifier_flag(left_shift_1);
      REQUIRE(*event_definition.test_modifiers(modifier_flag_manager) == std::unordered_set<krbn::modifier_flag>({
                                                                             krbn::modifier_flag::left_shift,
                                                                         }));
    }
    {
      krbn::modifier_flag_manager modifier_flag_manager;
      modifier_flag_manager.push_back_active_modifier_flag(left_shift_1);
      modifier_flag_manager.push_back_active_modifier_flag(right_shift_1);
      if (event_definition.test_modifiers(modifier_flag_manager) != boost::none) {
        REQUIRE(false);
      }
    }
  }

  {
    std::unordered_set<event_definition::modifier> modifiers({
        event_definition::modifier::shift,
    });
    event_definition event_definition(spacebar_key_code, modifiers);

    REQUIRE(*(event_definition.get_key_code()) == spacebar_key_code);
    REQUIRE(event_definition.get_modifiers() == modifiers);

    {
      krbn::modifier_flag_manager modifier_flag_manager;
      modifier_flag_manager.push_back_active_modifier_flag(left_command_1);
      if (event_definition.test_modifiers(modifier_flag_manager) != boost::none) {
        REQUIRE(false);
      }
    }
    {
      krbn::modifier_flag_manager modifier_flag_manager;
      modifier_flag_manager.push_back_active_modifier_flag(left_shift_1);
      REQUIRE(*event_definition.test_modifiers(modifier_flag_manager) == std::unordered_set<krbn::modifier_flag>({
                                                                             krbn::modifier_flag::left_shift,
                                                                         }));
    }
    {
      krbn::modifier_flag_manager modifier_flag_manager;
      modifier_flag_manager.push_back_active_modifier_flag(right_shift_1);
      REQUIRE(*event_definition.test_modifiers(modifier_flag_manager) == std::unordered_set<krbn::modifier_flag>({
                                                                             krbn::modifier_flag::right_shift,
                                                                         }));
    }
    {
      krbn::modifier_flag_manager modifier_flag_manager;
      modifier_flag_manager.push_back_active_modifier_flag(left_shift_1);
      modifier_flag_manager.push_back_active_modifier_flag(right_shift_1);
      REQUIRE(*event_definition.test_modifiers(modifier_flag_manager) == std::unordered_set<krbn::modifier_flag>({
                                                                             krbn::modifier_flag::left_shift,
                                                                         }));
    }
    {
      krbn::modifier_flag_manager modifier_flag_manager;
      modifier_flag_manager.push_back_active_modifier_flag(left_command_1);
      modifier_flag_manager.push_back_active_modifier_flag(left_shift_1);
      modifier_flag_manager.push_back_active_modifier_flag(right_shift_1);
      if (event_definition.test_modifiers(modifier_flag_manager) != boost::none) {
        REQUIRE(false);
      }
    }
  }

  {
    std::unordered_set<event_definition::modifier> modifiers({
        event_definition::modifier::shift,
        event_definition::modifier::any,
    });
    event_definition event_definition(spacebar_key_code, modifiers);

    REQUIRE(*(event_definition.get_key_code()) == spacebar_key_code);
    REQUIRE(event_definition.get_modifiers() == modifiers);

    {
      krbn::modifier_flag_manager modifier_flag_manager;
      modifier_flag_manager.push_back_active_modifier_flag(left_command_1);
      if (event_definition.test_modifiers(modifier_flag_manager) != boost::none) {
        REQUIRE(false);
      }
    }
    {
      krbn::modifier_flag_manager modifier_flag_manager;
      modifier_flag_manager.push_back_active_modifier_flag(left_shift_1);
      REQUIRE(*event_definition.test_modifiers(modifier_flag_manager) == std::unordered_set<krbn::modifier_flag>({
                                                                             krbn::modifier_flag::left_shift,
                                                                         }));
    }
    {
      krbn::modifier_flag_manager modifier_flag_manager;
      modifier_flag_manager.push_back_active_modifier_flag(right_shift_1);
      REQUIRE(*event_definition.test_modifiers(modifier_flag_manager) == std::unordered_set<krbn::modifier_flag>({
                                                                             krbn::modifier_flag::right_shift,
                                                                         }));
    }
    {
      krbn::modifier_flag_manager modifier_flag_manager;
      modifier_flag_manager.push_back_active_modifier_flag(left_shift_1);
      modifier_flag_manager.push_back_active_modifier_flag(right_shift_1);
      REQUIRE(*event_definition.test_modifiers(modifier_flag_manager) == std::unordered_set<krbn::modifier_flag>({
                                                                             krbn::modifier_flag::left_shift,
                                                                         }));
    }
    {
      krbn::modifier_flag_manager modifier_flag_manager;
      modifier_flag_manager.push_back_active_modifier_flag(left_command_1);
      modifier_flag_manager.push_back_active_modifier_flag(left_shift_1);
      modifier_flag_manager.push_back_active_modifier_flag(right_shift_1);
      REQUIRE(*event_definition.test_modifiers(modifier_flag_manager) == std::unordered_set<krbn::modifier_flag>({
                                                                             krbn::modifier_flag::left_shift,
                                                                         }));
    }
  }
}

int main(int argc, char* const argv[]) {
  krbn::thread_utility::register_main_thread();
  return Catch::Session().run(argc, argv);
}
