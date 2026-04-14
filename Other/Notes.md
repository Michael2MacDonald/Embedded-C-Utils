## Array generation with dynamic compile time registration

#### TODO: RENAME (Check markdown comments)

<!-- linker_lists -->

Allow fixed length lists of unknown length to created at compile time through
multiple calls to an 'append' macro.

Use `__attribute__((section("my_list")))` to create the list



Take a look at u-boot linker_lists: 
[https://github.com/u-boot/u-boot/blob/master/include/linker_lists.h] 
[https://docs.u-boot.org/en/latest/api/linker_lists.html]

A simple library inspired by u-boot linker_lists:
[https://github.com/attie/linker-generated-array/blob/master/lga.h]