User Relationships Services
==============

user_relationships_services is a Drupal (http://drupal.org) module which exposes methods, to create, maintain, destroy user relationships, to services module.

This is a complete rewrite and a different version from the original "user_relationship_services" module in the user_relationships suite.

Requirements
--------

 * http://drupal.org/project/user_relationships
 * http://drupal.org/project/services

Services provided
--------

1. user_relationships.types

    Input: None.
    Output: (array) rids [with information].

2. user_relationships.list

    Input: None.
    Output: (array) user objects [with rtid, rid and relationship status for each].

3. user_relationships.request

    Input: (string) requestee email, (int) requester uid, (string) type.
    Output: (int) rid.

4. user_relationships.approve

    Input: (int) rid.
    Output: None.

5. user_relationships.remind

    Input: (int) rid.
    Output: None.

6. user_relationships.delete

    Input: (int) rid, (string) reason [options: cancel, disapprove, remove].
    Output: None.


License
-------

Copyright (c) 2010 Gurpartap Singh, http://gurpartap.com/

This code is licensed under the MIT License

You are free:

 * to Share — to copy, distribute and transmit the work
 * to Remix — to adapt the work

Under the following conditions:

 * The copyright notice and license shall be included in all copies or substantial portions of the software.
 * Any of the above conditions can be waived if you get permission from the copyright holder.

See bundled MIT-LICENSE.txt file for detailed license terms.