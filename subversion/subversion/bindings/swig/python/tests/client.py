#
#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.
#
#
import unittest, os, weakref, setup_path, utils

from svn import core, client, wc

try:
  # Python >=3.0
  from urllib.parse import urljoin
except ImportError:
  # Python <3.0
  from urlparse import urljoin

class SubversionClientTestCase(unittest.TestCase):
  """Test cases for the basic SWIG Subversion client layer"""

  def log_message_func(self, items, pool):
    """ Simple log message provider for unit tests. """
    self.log_message_func_calls += 1
    return "Test log message"

  def log_receiver(self, changed_paths, revision, author, date, message, pool):
    """ Function to receive log messages retrieved by client.log3(). """
    self.log_message = message
    self.change_author = author
    self.changed_paths = changed_paths

  def log_entry_receiver(self, log_entry, pool):
    """An implementation of svn_log_entry_receiver_t."""
    self.received_revisions.append(log_entry.revision)

  def setUp(self):
    """Set up authentication and client context"""
    self.client_ctx = client.svn_client_create_context()
    self.assertEquals(self.client_ctx.log_msg_baton2, None)
    self.assertEquals(self.client_ctx.log_msg_func2, None)
    self.assertEquals(self.client_ctx.log_msg_baton3, None)
    self.assertEquals(self.client_ctx.log_msg_func3, None)
    self.client_ctx.log_msg_func3 = client.svn_swig_py_get_commit_log_func
    self.client_ctx.log_msg_baton3 = self.log_message_func
    self.log_message_func_calls = 0
    self.log_message = None
    self.changed_paths = None
    self.change_author = None

    providers = [
       client.svn_client_get_simple_provider(),
       client.svn_client_get_username_provider(),
    ]

    self.client_ctx.auth_baton = core.svn_auth_open(providers)

    self.temper = utils.Temper()
    (_, self.repos_path, self.repos_uri) = self.temper.alloc_known_repo(
      'trac/versioncontrol/tests/svnrepos.dump', suffix='-client')

  def tearDown(self):
    # We have to free client_ctx first, since it may be holding handles
    # to WC DBs
    del self.client_ctx
    self.temper.cleanup()

  def testBatonPlay(self):
    """Test playing with C batons"""
    baton = lambda: 1
    weakref_baton = weakref.ref(baton)
    self.client_ctx.log_msg_baton2 = baton
    baton = None
    self.assertEquals(self.client_ctx.log_msg_baton2(), 1)
    self.assertEquals(weakref_baton()(), 1)
    self.client_ctx.log_msg_baton2 = None
    self.assertEquals(self.client_ctx.log_msg_baton2, None)
    self.assertEquals(weakref_baton(), None)

    # External objects should retain their current parent pool
    self.assertNotEquals(self.client_ctx._parent_pool,
                         self.client_ctx.auth_baton._parent_pool)

    # notify_func2 and notify_baton2 were generated by
    # svn_client_create_context, so they should have
    # the same pool as the context
    self.assertEquals(self.client_ctx._parent_pool,
                      self.client_ctx.notify_func2._parent_pool)
    self.assertEquals(self.client_ctx._parent_pool,
                      self.client_ctx.notify_baton2._parent_pool)

  def testMethodCalls(self):
    """Test direct method calls to callbacks"""

    # Directly invoking the msg_baton should work
    self.client_ctx.log_msg_baton3(None, None)
    b = self.client_ctx.log_msg_baton3
    b(None, None)
    self.assertEqual(self.log_message_func_calls, 2)

    # You can also invoke the log_msg_func3. It'd be
    # nice if we could get log_msg_func3 function
    # to invoke the baton function, but, in order to do that,
    # we'd need to supply a value for the first parameter.
    self.client_ctx.log_msg_func3(None, self.client_ctx.log_msg_baton3)

  def info_receiver(self, path, info, pool):
    """Squirrel away the output from 'svn info' so that the unit tests
       can get at them."""
    self.path = path
    self.info = info

  def test_client_ctx_baton_lifetime(self):
    pool = core.Pool()
    temp_client_ctx = client.svn_client_create_context(pool)

    # We keep track of these objects in separate variables here
    # because you can't get a PyObject back out of a PY_AS_VOID field
    test_object1 = lambda *args: "message 1"
    test_object2 = lambda *args: "message 2"

    # Verify that the refcount of a Python object is incremented when
    # you insert it into a PY_AS_VOID field.
    temp_client_ctx.log_msg_baton2 = test_object1
    test_object1 = weakref.ref(test_object1)
    self.assertNotEqual(test_object1(), None)

    # Verify that the refcount of the previous Python object is decremented
    # when a PY_AS_VOID field is replaced.
    temp_client_ctx.log_msg_baton2 = test_object2
    self.assertEqual(test_object1(), None)

    # Verify that the reference count of the new Python object (which
    # replaced test_object1) was incremented.
    test_object2 = weakref.ref(test_object2)
    self.assertNotEqual(test_object2(), None)

    # Verify that the reference count of test_object2 is decremented when
    # test_client_ctx is destroyed.
    temp_client_ctx = None
    self.assertEqual(test_object2(), None)

  def test_checkout(self):
    """Test svn_client_checkout2."""

    rev = core.svn_opt_revision_t()
    rev.kind = core.svn_opt_revision_head

    path = self.temper.alloc_empty_dir('-checkout')

    self.assertRaises(ValueError, client.checkout2,
                      self.repos_uri, path, None, None, True, True,
                      self.client_ctx)

    client.checkout2(self.repos_uri, path, rev, rev, True, True,
            self.client_ctx)

  def test_info(self):
    """Test svn_client_info on an empty repository"""

    # Run info
    revt = core.svn_opt_revision_t()
    revt.kind = core.svn_opt_revision_head
    client.info(self.repos_uri, revt, revt, self.info_receiver,
                False, self.client_ctx)

    # Check output from running info. This also serves to verify that
    # the internal 'info' object is still valid
    self.assertEqual(self.path, os.path.basename(self.repos_path))
    self.info.assert_valid()
    self.assertEqual(self.info.URL, self.repos_uri)
    self.assertEqual(self.info.repos_root_URL, self.repos_uri)

  def test_mkdir_url(self):
    """Test svn_client_mkdir2 on a file:// URL"""
    directory = urljoin(self.repos_uri+"/", "dir1")

    commit_info = client.mkdir2((directory,), self.client_ctx)
    self.assertEqual(commit_info.revision, 13)
    self.assertEqual(self.log_message_func_calls, 1)

  def test_mkdir_url_with_revprops(self):
    """Test svn_client_mkdir3 on a file:// URL, with added revprops"""
    directory = urljoin(self.repos_uri+"/", "some/deep/subdir")

    commit_info = client.mkdir3((directory,), 1, {'customprop':'value'},
                                self.client_ctx)
    self.assertEqual(commit_info.revision, 13)
    self.assertEqual(self.log_message_func_calls, 1)

  def test_log3_url(self):
    """Test svn_client_log3 on a file:// URL"""
    directory = urljoin(self.repos_uri+"/", "trunk/dir1")

    start = core.svn_opt_revision_t()
    end = core.svn_opt_revision_t()
    core.svn_opt_parse_revision(start, end, "4:0")
    client.log3((directory,), start, start, end, 1, True, False,
        self.log_receiver, self.client_ctx)
    self.assertEqual(self.change_author, "john")
    self.assertEqual(self.log_message, "More directories.")
    self.assertEqual(len(self.changed_paths), 3)
    for dir in ('/trunk/dir1', '/trunk/dir2', '/trunk/dir3'):
      self.assert_(dir in self.changed_paths)
      self.assertEqual(self.changed_paths[dir].action, 'A')

  def test_log5(self):
    """Test svn_client_log5."""
    start = core.svn_opt_revision_t()
    start.kind = core.svn_opt_revision_number
    start.value.number = 0

    end = core.svn_opt_revision_t()
    end.kind = core.svn_opt_revision_number
    end.value.number = 4

    rev_range = core.svn_opt_revision_range_t()
    rev_range.start = start
    rev_range.end = end

    self.received_revisions = []

    client.log5((self.repos_uri,), end, (rev_range,), 0, False, True, False, (),
        self.log_entry_receiver, self.client_ctx)

    self.assertEqual(self.received_revisions, range(0, 5))

  def test_uuid_from_url(self):
    """Test svn_client_uuid_from_url on a file:// URL"""
    self.assert_(isinstance(
                 client.uuid_from_url(self.repos_uri, self.client_ctx),
                 basestring))

  def test_url_from_path(self):
    """Test svn_client_url_from_path for a file:// URL"""
    self.assertEquals(client.url_from_path(self.repos_uri), self.repos_uri)

    rev = core.svn_opt_revision_t()
    rev.kind = core.svn_opt_revision_head

    path = self.temper.alloc_empty_dir('-url_from_path')

    client.checkout2(self.repos_uri, path, rev, rev, True, True,
                     self.client_ctx)

    self.assertEquals(client.url_from_path(path), self.repos_uri)

  def test_uuid_from_path(self):
    """Test svn_client_uuid_from_path."""
    rev = core.svn_opt_revision_t()
    rev.kind = core.svn_opt_revision_head

    path = self.temper.alloc_empty_dir('-uuid_from_path')

    client.checkout2(self.repos_uri, path, rev, rev, True, True,
                     self.client_ctx)

    wc_adm = wc.adm_open3(None, path, False, 0, None)

    self.assertEquals(client.uuid_from_path(path, wc_adm, self.client_ctx),
                      client.uuid_from_url(self.repos_uri, self.client_ctx))

    self.assert_(isinstance(client.uuid_from_path(path, wc_adm,
                            self.client_ctx), basestring))

  def test_open_ra_session(self):
      """Test svn_client_open_ra_session()."""
      client.open_ra_session(self.repos_uri, self.client_ctx)


  def test_info_file(self):
    """Test svn_client_info on working copy file and remote files."""

    # This test requires a file /trunk/README.txt of size 8 bytes
    # in the repository.
    rev = core.svn_opt_revision_t()
    rev.kind = core.svn_opt_revision_head
    wc_path = self.temper.alloc_empty_dir('-info_file')

    client.checkout2(self.repos_uri, wc_path, rev, rev, True, True,
                     self.client_ctx)
    adm_access = wc.adm_open3(None, wc_path, True, -1, None)

    try:
      # Test 1: Run info -r BASE. We expect the size value to be filled in.
      rev.kind = core.svn_opt_revision_base
      readme_path = '%s/trunk/README.txt' % wc_path
      readme_url = '%s/trunk/README.txt' % self.repos_uri
      client.info(readme_path, rev, rev, self.info_receiver,
                  False, self.client_ctx)

      self.assertEqual(self.path, os.path.basename(readme_path))
      self.info.assert_valid()
      self.assertEqual(self.info.working_size, client.SWIG_SVN_INFO_SIZE_UNKNOWN)
      self.assertEqual(self.info.size, 8)

      # Test 2: Run info (revision unspecified). We expect the working_size value
      # to be filled in.
      rev.kind = core.svn_opt_revision_unspecified
      client.info(readme_path, rev, rev, self.info_receiver,
                  False, self.client_ctx)

      self.assertEqual(self.path, readme_path)
      self.info.assert_valid()
      self.assertEqual(self.info.size, client.SWIG_SVN_INFO_SIZE_UNKNOWN)
      # README.txt contains one EOL char, so on Windows it will be expanded from
      # LF to CRLF hence the working_size will be 9 instead of 8.
      if os.name == 'nt':
        self.assertEqual(self.info.working_size, 9)
      else:
        self.assertEqual(self.info.working_size, 8)

      # Test 3: Run info on the repository URL of README.txt. We expect the size
      # value to be filled in.
      rev.kind = core.svn_opt_revision_head
      client.info(readme_url, rev, rev, self.info_receiver,
                  False, self.client_ctx)
      self.info.assert_valid()
      self.assertEqual(self.info.working_size, client.SWIG_SVN_INFO_SIZE_UNKNOWN)
      self.assertEqual(self.info.size, 8)
    finally:
      wc.adm_close(adm_access)

  def test_merge_peg3(self):
    """Test svn_client_merge_peg3."""
    head = core.svn_opt_revision_t()
    head.kind = core.svn_opt_revision_head
    wc_path = self.temper.alloc_empty_dir('-merge_peg3')

    client.checkout3(self.repos_uri, wc_path, head, head, core.svn_depth_infinity,
                     True, False, self.client_ctx)

    # Let's try to backport a change from the v1x branch
    trunk_path = core.svn_dirent_join(wc_path, 'trunk')
    v1x_path = core.svn_dirent_join(wc_path, 'branches/v1x')

    start = core.svn_opt_revision_t()
    start.kind = core.svn_opt_revision_number
    start.value.number = 8

    end = core.svn_opt_revision_t()
    end.kind = core.svn_opt_revision_number
    end.value.number = 9

    rrange = core.svn_opt_revision_range_t()
    rrange.start = start
    rrange.end = end

    client.merge_peg3(v1x_path, (rrange,), end, trunk_path,
                      core.svn_depth_infinity, False, False, False, False,
                      None, self.client_ctx)

    # Did it take effect?
    readme_path_native = core.svn_dirent_local_style(
      core.svn_dirent_join(trunk_path, 'README.txt')
    )

    readme = open(readme_path_native, 'r')
    readme_text = readme.read()
    readme.close()

    self.assertEqual(readme_text, 'This is a test.\n')

def suite():
    return unittest.defaultTestLoader.loadTestsFromTestCase(
      SubversionClientTestCase)

if __name__ == '__main__':
    runner = unittest.TextTestRunner()
    runner.run(suite())

