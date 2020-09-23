import unittest
import server_tests

test_runner = unittest.TestSuite()
test_runner.addTest(unittest.makeSuite(server_tests.TestServerReg))
test_runner.addTest(unittest.makeSuite(server_tests.TestServerInput))

runner = unittest.TextTestRunner(verbosity=2)
runner.run(test_runner)