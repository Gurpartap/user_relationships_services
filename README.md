User Relationships Services
==============

user_relationships_services is a Drupal (http://drupal.org) module which exposes methods, to create, maintain, destroy user relationships, to services module.

This is a complete rewrite of the original user_relationship_services module in the user_relationships (http://drupal.org/project/user_relationships) suite.

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